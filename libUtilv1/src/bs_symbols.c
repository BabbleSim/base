/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "bs_types.h"
#include "bs_tracing.h"
#include "bs_symbols.h"
#include "bs_oswrap.h"
#include "bs_utils.h"

/***********************************************************
 * Functions to resolve symbols names from their addresses *
 ***********************************************************/

#define SymbolNameSize 64
typedef struct {
  void* Offset;
  char FName[SymbolNameSize];
} symbol_entry_t;

symbol_entry_t *functions_table = NULL;
uint Number_Symbols = 0;

#if (_BS_TSYMBOLS_TRACE)
/**
 * Load the Tsymbols file into memory
 * Note: this is quite unreliable (very OS and toolchain dependent),
 * but the functionality it provides is quite superfluous (debugging aid)
 * Therefore it is meant to fail silently
 */
void bs_read_function_names_from_Tsymbols(const char *probable_binary_name){
  if ( probable_binary_name != NULL ) {
#define TSymbolsNameSize 256
    char symbols_fname[TSymbolsNameSize];
    snprintf(symbols_fname, TSymbolsNameSize, "%s.Tsymbols", probable_binary_name);

    FILE *symbols_file = fopen(symbols_fname,"rt");
    if ( symbols_file == NULL ) {
      return;
    }
    uint nbr_symbols = 0;
    uint read_symbols = 0;
    uint error = 0;
    if ( fscanf(symbols_file, "%u\n", &nbr_symbols) != 1 ) {
      fclose(symbols_file);
      return;
    }
    functions_table = bs_calloc(nbr_symbols, sizeof(symbol_entry_t));

    while ( !feof(symbols_file) && ( read_symbols < nbr_symbols ) ){
      int read = fscanf(symbols_file, "%p", &functions_table[read_symbols].Offset);
      char endofline;

      if ( (read == 0) || ( read == EOF) ) {
        error = 1;
        break;
      }

      read = fscanf(symbols_file, "%"STR(SymbolNameSize)"s%c", functions_table[read_symbols].FName, &endofline);
      if ( (read == 0) || ( read == EOF) ){
        error = 2;
        break;
      }
      if (endofline != '\n'){
        bs_skipline(symbols_file); //disregard the rest of the function name
      }
      read_symbols++;
    }
    if ( error || ( nbr_symbols != read_symbols )) {
      bs_trace_warning_line("%s seems to be corrupted (%i) (Expected %u entries, read so far %u)\n",symbols_fname, error, nbr_symbols, read_symbols);
      if (error){
        free(functions_table);
        functions_table = NULL;
      }
    }
    Number_Symbols = read_symbols;
    fclose(symbols_file);
  }
}
#endif /* _BS_TSYMBOLS_TRACE */

void bs_clear_Tsymbols(){
  if ( functions_table != NULL ) {
    free(functions_table);
    functions_table = NULL;
  }
}


/**
 * For a given ptr, tell if this could actually be a symbol (or an offset relative to a symbol) == 1
 * or not == 0
 */
int bs_could_be_symbol(uint32_t ptr){
#if (_BS_TSYMBOLS_TRACE )
  if ( functions_table == NULL )
    return 0;
  if ( ( ((intptr_t) ptr) < ((intptr_t)functions_table[0].Offset) )
      || ( ((intptr_t) ptr) > ((intptr_t)functions_table[Number_Symbols-1].Offset) ) ) { //if it is smaller than _init or greater than _end, this is not a symbol
    return 0;
  }
  return 1;
#else
  return 0;
#endif
}

/*
 * For a given pointer, return the entry in functions_table which contains it (+ the Offset)
 */
static void bs_find_symbol_name(void *fptr, uint *EntryNbr, intptr_t *Offset){
  *EntryNbr = 0;

  {// we search for the last entry smaller than fptr
    uint Step = ( Number_Symbols + 1 )/ 2;
    int Trial = Step;
    uint CloserEntry = 0;
    int64_t CloserDistance = INT64_MAX;

    int64_t distance;
    while ( Step != 0 ){ //A search thru bisection
      distance = ((intptr_t)fptr) - ((intptr_t)functions_table[Trial].Offset);

      if ( Step > 1 ) {
        Step = ( Step + 1 )/ 2;
      } else {
        Step = 0;
      }

      if ( distance < 0 ) {
        Trial = BS_MAX(Trial-(int)Step,0);
      } else if ( distance > 0 ) {
        if ( distance < CloserDistance ){
          CloserDistance = distance;
          CloserEntry = Trial;
        }
        Trial = BS_MIN(Trial+Step,Number_Symbols-1);
      } else if ( distance == 0 ) {
        CloserDistance = distance;
        CloserEntry = Trial;
        break;
      }
    }

    *EntryNbr = CloserEntry;
  }

  *Offset = ( (intptr_t) fptr ) - (intptr_t)functions_table[*EntryNbr].Offset;
}

/*
 * Print to a string the name of a function/variable given its pointer as in:
 *  (Symbol_Name+Offset) [Absolute Address in Memory]
 */
void bs_snprint_symbol_name_from_Tsymbols(void *fptr, char* ptr, size_t n){
  if ( functions_table == NULL ){
    snprintf(ptr,n, "%p",fptr);
  } else {
    uint EntryNbr;
    intptr_t Offset;
    bs_find_symbol_name(fptr, &EntryNbr, &Offset);
    if (Offset == 0) {
      snprintf(ptr,n, "(%s) [%p]",functions_table[EntryNbr].FName, fptr);
    } else {
      snprintf(ptr,n, "(%s + %p) [%p]",functions_table[EntryNbr].FName, (void*)Offset, fptr);
    }
  }
}
