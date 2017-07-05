/**********************************************************************/
/*                                                                    */
/*             FILENAME:  lex.h                                       */
/*             BY:  Jonathan G. Fiscus                                */
/*                  NATIONAL INSTITUTE OF STANDARDS AND TECHNOLOGY    */
/*                  SPEECH RECOGNITION GROUP                          */
/*                                                                    */
/*           DESC:  This file contains typedefs and macros for usage  */
/*                  of a LEXICON structure                            */
/*                                                                    */
/**********************************************************************/
#define WORD_NOT_IN_LEX		(-1)
#define MAX_LEX_WORD_LEN	50

#define lex_word(_l,_w)		_l->cd[1].pc->pc[_w].ac
#define lex_num(_l)		_l->cd[1].pc->pctab_length
#define lex_lc_word(_l,_w)	str2low(lex_word(_l,_w))
#define lex_uc_word(_l,_w)	str2up(lex_word(_l,_w))


