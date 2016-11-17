// A Bison parser, made by GNU Bison 3.0.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2013 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.
// //                    "%code top" blocks.


#include "stdafx.h"

#include <ace/Synch.h>
#include "bittorrent_metainfo_parser.h"




// First part of user declarations.



# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "bittorrent_metainfo_parser.h"

// User implementation prologue.


// Unqualified %code blocks.


// *NOTE*: necessary only if %debug is set in the definition file (see: parser.y)
/*#if defined (YYDEBUG)
#include <iostream>
#endif*/
/*#include <regex>*/
#include <sstream>
#include <string>

// *WORKAROUND*
/*using namespace std;*/
// *IMPORTANT NOTE*: several ACE headers inclue ace/iosfwd.h, which introduces
//                   a problem in conjunction with the standard include headers
//                   when ACE_USES_OLD_IOSTREAMS is defined
//                   --> include the necessary headers manually (see above), and
//                       prevent ace/iosfwd.h from causing any harm
/*#define ACE_IOSFWD_H*/

#include <ace/Log_Msg.h>
#include <ace/OS.h>

#include "net_macros.h"

#include "bencoding_scanner.h"
#include "bittorrent_common.h"
#include "bittorrent_defines.h"
/*#include <ace/Synch.h>*/
#include "bittorrent_metainfo_parser_driver.h"
#include "bittorrent_tools.h"

// *TODO*: this shouldn't be necessary
/*#define yylex bencoding_lex*/
#define yylex scanner->yylex

//#define YYPRINT(file, type, value) yyprint (file, type, value)




#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (/*CONSTCOND*/ false)
# endif


// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << std::endl;                  \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE(Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void>(0)
# define YY_STACK_PRINT()                static_cast<void>(0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyempty = true)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


namespace yy {


  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  BitTorrent_MetaInfo_Parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr = "";
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              // Fall through.
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }


  /// Build a parser object.
  BitTorrent_MetaInfo_Parser::BitTorrent_MetaInfo_Parser (BitTorrent_MetaInfo_IParser_t* parser_yyarg, Net_BencodingScanner* scanner_yyarg, std::string* dictionary_key_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      parser (parser_yyarg),
      scanner (scanner_yyarg),
      dictionary_key (dictionary_key_yyarg)
  {}

  BitTorrent_MetaInfo_Parser::~BitTorrent_MetaInfo_Parser ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/



  // by_state.
  inline
  BitTorrent_MetaInfo_Parser::by_state::by_state ()
    : state (empty)
  {}

  inline
  BitTorrent_MetaInfo_Parser::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  inline
  void
  BitTorrent_MetaInfo_Parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.state = empty;
  }

  inline
  BitTorrent_MetaInfo_Parser::by_state::by_state (state_type s)
    : state (s)
  {}

  inline
  BitTorrent_MetaInfo_Parser::symbol_number_type
  BitTorrent_MetaInfo_Parser::by_state::type_get () const
  {
    return state == empty ? 0 : yystos_[state];
  }

  inline
  BitTorrent_MetaInfo_Parser::stack_symbol_type::stack_symbol_type ()
  {}


  inline
  BitTorrent_MetaInfo_Parser::stack_symbol_type::stack_symbol_type (state_type s, symbol_type& that)
    : super_type (s, that.location)
  {
      switch (that.type_get ())
    {
      case 7: // "list"
        value.move< BitTorrent_MetaInfo_List_t > (that.value);
        break;

      case 6: // "integer"
        value.move< int > (that.value);
        break;

      case 5: // "string"
        value.move< std::string > (that.value);
        break;

      case 9: // metainfo
      case 10: // list_items
      case 11: // list_item
      case 12: // dictionary_items
      case 13: // dictionary_item
      case 14: // dictionary_value
        value.move< unsigned int > (that.value);
        break;

      default:
        break;
    }

    // that is emptied.
    that.type = empty;
  }

  inline
  BitTorrent_MetaInfo_Parser::stack_symbol_type&
  BitTorrent_MetaInfo_Parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
      switch (that.type_get ())
    {
      case 7: // "list"
        value.copy< BitTorrent_MetaInfo_List_t > (that.value);
        break;

      case 6: // "integer"
        value.copy< int > (that.value);
        break;

      case 5: // "string"
        value.copy< std::string > (that.value);
        break;

      case 9: // metainfo
      case 10: // list_items
      case 11: // list_item
      case 12: // dictionary_items
      case 13: // dictionary_item
      case 14: // dictionary_value
        value.copy< unsigned int > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }


  template <typename Base>
  inline
  void
  BitTorrent_MetaInfo_Parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  BitTorrent_MetaInfo_Parser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    switch (yytype)
    {
            case 5: // "string"


        { yyoutput << yysym.value.template as< std::string > (); }

        break;

      case 7: // "list"


        { for (BitTorrent_MetaInfo_ListIterator_t iterator = yysym.value.template as< BitTorrent_MetaInfo_List_t > ().begin();
                   iterator != yysym.value.template as< BitTorrent_MetaInfo_List_t > ().end ();
                   ++iterator)
                yyoutput << *iterator << ','; }

        break;

      case 9: // metainfo


        { yyoutput << yysym.value.template as< unsigned int > (); }

        break;

      case 10: // list_items


        { yyoutput << yysym.value.template as< unsigned int > (); }

        break;

      case 11: // list_item


        { yyoutput << yysym.value.template as< unsigned int > (); }

        break;

      case 12: // dictionary_items


        { yyoutput << yysym.value.template as< unsigned int > (); }

        break;

      case 13: // dictionary_item


        { yyoutput << yysym.value.template as< unsigned int > (); }

        break;

      case 14: // dictionary_value


        { yyoutput << yysym.value.template as< unsigned int > (); }

        break;


      default:
        break;
    }
    yyo << ')';
  }
#endif

  inline
  void
  BitTorrent_MetaInfo_Parser::yypush_ (const char* m, state_type s, symbol_type& sym)
  {
    stack_symbol_type t (s, sym);
    yypush_ (m, t);
  }

  inline
  void
  BitTorrent_MetaInfo_Parser::yypush_ (const char* m, stack_symbol_type& s)
  {
    if (m)
      YY_SYMBOL_PRINT (m, s);
    yystack_.push (s);
  }

  inline
  void
  BitTorrent_MetaInfo_Parser::yypop_ (unsigned int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  BitTorrent_MetaInfo_Parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  BitTorrent_MetaInfo_Parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  BitTorrent_MetaInfo_Parser::debug_level_type
  BitTorrent_MetaInfo_Parser::debug_level () const
  {
    return yydebug_;
  }

  void
  BitTorrent_MetaInfo_Parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  inline BitTorrent_MetaInfo_Parser::state_type
  BitTorrent_MetaInfo_Parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  inline bool
  BitTorrent_MetaInfo_Parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  BitTorrent_MetaInfo_Parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  BitTorrent_MetaInfo_Parser::parse ()
  {
    /// Whether yyla contains a lookahead.
    bool yyempty = true;

    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


    // User initialization code.
    
{
  // initialize the location
  yyla.location.initialize (YY_NULLPTR, 1, 1);

  // sanity check(s)
  ACE_ASSERT (dictionary_key);
/*  std::string dictionary_key;*/
}



    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, yyla);

    // A new symbol was pushed on the stack.
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << std::endl;

    // Accept?
    if (yystack_[0].state == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    // Backup.
  yybackup:

    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyempty)
      {
        YYCDEBUG << "Reading a token: ";
        try
          {
            symbol_type yylookahead (yylex (parser));
            yyla.move (yylookahead);
          }
        catch (const syntax_error& yyexc)
          {
            error (yyexc);
            goto yyerrlab1;
          }
        yyempty = false;
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Discard the token being shifted.
    yyempty = true;

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, yyla);
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_(yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
        switch (yyr1_[yyn])
    {
      case 7: // "list"
        yylhs.value.build< BitTorrent_MetaInfo_List_t > ();
        break;

      case 6: // "integer"
        yylhs.value.build< int > ();
        break;

      case 5: // "string"
        yylhs.value.build< std::string > ();
        break;

      case 9: // metainfo
      case 10: // list_items
      case 11: // list_item
      case 12: // dictionary_items
      case 13: // dictionary_item
      case 14: // dictionary_value
        yylhs.value.build< unsigned int > ();
        break;

      default:
        break;
    }


      // Compute the default @$.
      {
        slice<stack_symbol_type, stack_type> slice (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, slice, yylen);
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
      try
        {
          switch (yyn)
            {
  case 2:

    { yylhs.value.as< unsigned int > () = yystack_[0].value.as< unsigned int > ();
                                     struct BitTorrent_MetaInfo& metainfo_r =
                                       parser->current ();
                                     struct BitTorrent_MetaInfo* metainfo_p =
                                       &metainfo_r;
                                     try {
                                       parser->record (metainfo_p);
                                     } catch (...) {
                                       ACE_DEBUG ((LM_ERROR,
                                                   ACE_TEXT ("caught exception in BitTorrent_MetaInfo_IParser::record(), continuing\n")));
                                     } }

    break;

  case 3:

    { yylhs.value.as< unsigned int > () = yystack_[1].value.as< unsigned int > () + yystack_[0].value.as< unsigned int > (); }

    break;

  case 4:

    { yylhs.value.as< unsigned int > () = 0; }

    break;

  case 5:

    { yylhs.value.as< unsigned int > () = yystack_[0].value.as< std::string > ().size ();

                                 struct BitTorrent_MetaInfo& metainfo_r =
                                   parser->current ();
                                 if (*dictionary_key == ACE_TEXT_ALWAYS_CHAR ("announce-list"))
                                 {
                                   BitTorrent_MetaInfo_AnnounceList_t& current_r =
                                     metainfo_r.announceList.back ();
                                   current_r.push_back (yystack_[0].value.as< std::string > ());
                                 } }

    break;

  case 6:

    { yylhs.value.as< unsigned int > () = 0; }

    break;

  case 7:

    { yylhs.value.as< unsigned int > () = yystack_[0].value.as< unsigned int > ();

                                        struct BitTorrent_MetaInfo& metainfo_r =
                                          const_cast<struct BitTorrent_MetaInfo&> (parser->current ());
                                        if (*dictionary_key == ACE_TEXT_ALWAYS_CHAR ("announce-list"))
                                        {
                                          BitTorrent_MetaInfo_AnnounceList_t announce_list;
                                          metainfo_r.announceList.push_back (announce_list);
                                        } }

    break;

  case 8:

    { yylhs.value.as< unsigned int > () = yystack_[0].value.as< unsigned int > (); }

    break;

  case 9:

    { yylhs.value.as< unsigned int > () = yystack_[1].value.as< unsigned int > () + yystack_[0].value.as< unsigned int > (); }

    break;

  case 10:

    { yylhs.value.as< unsigned int > () = 0; }

    break;

  case 11:

    { yylhs.value.as< unsigned int > () = yystack_[1].value.as< std::string > ().size () + yystack_[0].value.as< unsigned int > ();
                                              *dictionary_key = yystack_[1].value.as< std::string > (); }

    break;

  case 12:

    { yylhs.value.as< unsigned int > () = yystack_[0].value.as< std::string > ().size ();
                             ACE_DEBUG ((LM_DEBUG,
                                         ACE_TEXT ("key: \"%s\": \"%s\"\n"),
                                         ACE_TEXT (dictionary_key->c_str ()),
                                         ACE_TEXT (yystack_[0].value.as< std::string > ().c_str ())));

                             struct BitTorrent_MetaInfo& metainfo_r =
                               parser->current ();
                             if (metainfo_r.singleFileMode)
                             { ACE_ASSERT (metainfo_r.info.single_file);
                               if (*dictionary_key == ACE_TEXT_ALWAYS_CHAR ("pieces"))
                                 metainfo_r.info.single_file->pieces = yystack_[0].value.as< std::string > ();
                               else if (*dictionary_key == ACE_TEXT_ALWAYS_CHAR ("name"))
                                 metainfo_r.info.single_file->name = yystack_[0].value.as< std::string > ();
                               else if (*dictionary_key == ACE_TEXT_ALWAYS_CHAR ("md5sum"))
                                 metainfo_r.info.single_file->md5sum = yystack_[0].value.as< std::string > ();
                             } // end ELSE IF
                             else
                             { ACE_ASSERT (metainfo_r.info.multiple_file);
                               if (*dictionary_key == ACE_TEXT_ALWAYS_CHAR ("pieces"))
                                 metainfo_r.info.multiple_file->pieces = yystack_[0].value.as< std::string > ();
                               else if (*dictionary_key == ACE_TEXT_ALWAYS_CHAR ("name"))
                                 metainfo_r.info.multiple_file->name = yystack_[0].value.as< std::string > ();
                               else if (*dictionary_key == ACE_TEXT_ALWAYS_CHAR ("md5sum"))
                               {
                                 struct BitTorrent_MetaInfo_InfoDictionary_MultipleFile_File& current_r =
                                   metainfo_r.info.multiple_file->files.back ();
                                 current_r.md5sum = yystack_[0].value.as< std::string > ();
                               } // end ELSE IF
                             } }

    break;

  case 13:

    { yylhs.value.as< unsigned int > () = yystack_[0].value.as< int > ();
                                ACE_DEBUG ((LM_DEBUG,
                                            ACE_TEXT ("key: \"%s\": %d\n"),
                                            ACE_TEXT (dictionary_key->c_str ()),
                                            yystack_[0].value.as< int > ()));

                                struct BitTorrent_MetaInfo& metainfo_r =
                                  parser->current ();

                                if (metainfo_r.singleFileMode)
                                { ACE_ASSERT (metainfo_r.info.single_file);
                                  if (*dictionary_key == ACE_TEXT_ALWAYS_CHAR ("piece length"))
                                    metainfo_r.info.single_file->pieceLength = yystack_[0].value.as< int > ();
                                  else if (*dictionary_key == ACE_TEXT_ALWAYS_CHAR ("private"))
                                    metainfo_r.info.single_file->_private = yystack_[0].value.as< int > ();
                                  else if (*dictionary_key == ACE_TEXT_ALWAYS_CHAR ("length"))
                                    metainfo_r.info.single_file->length = yystack_[0].value.as< int > ();
                                }
                                else
                                { ACE_ASSERT (metainfo_r.info.multiple_file);
                                  if (*dictionary_key == ACE_TEXT_ALWAYS_CHAR ("piece length"))
                                    metainfo_r.info.multiple_file->pieceLength = yystack_[0].value.as< int > ();
                                  else if (*dictionary_key == ACE_TEXT_ALWAYS_CHAR ("private"))
                                    metainfo_r.info.multiple_file->_private = yystack_[0].value.as< int > ();
                                  else if (*dictionary_key == ACE_TEXT_ALWAYS_CHAR ("length"))
                                  {
                                    struct BitTorrent_MetaInfo_InfoDictionary_MultipleFile_File& current_r =
                                      metainfo_r.info.multiple_file->files.back ();
                                    current_r.length = yystack_[0].value.as< int > ();
                                  }
                                } }

    break;

  case 14:

    { yylhs.value.as< unsigned int > () = yystack_[0].value.as< unsigned int > ();

                                        struct BitTorrent_MetaInfo& metainfo_r =
                                          parser->current ();

                                        if (metainfo_r.singleFileMode) {}
                                        else
                                        { ACE_ASSERT (metainfo_r.info.multiple_file);
                                          if (*dictionary_key == ACE_TEXT_ALWAYS_CHAR ("path"))
                                          {
                                            struct BitTorrent_MetaInfo_InfoDictionary_MultipleFile_File& current_r =
                                              metainfo_r.info.multiple_file->files.back ();
                                            BitTorrent_MetaInfo_ListIterator_t iterator =
                                              yystack_[1].value.as< BitTorrent_MetaInfo_List_t > ().begin ();
                                            current_r.path = *iterator;
                                            for (;
                                                 iterator != yystack_[1].value.as< BitTorrent_MetaInfo_List_t > ().end ();
                                                 ++iterator)
                                            {
                                              current_r.path += ACE_DIRECTORY_SEPARATOR_STR;
                                              current_r.path += *iterator;
                                            } // end FOR
                                          } // end IF
                                        } }

    break;

  case 15:

    { yylhs.value.as< unsigned int > () = yystack_[0].value.as< unsigned int > (); }

    break;



            default:
              break;
            }
        }
      catch (const syntax_error& yyexc)
        {
          error (yyexc);
          YYERROR;
        }
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, yylhs);
    }
    goto yynewstate;

  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yyla.location, yysyntax_error_ (yystack_[0].state,
                                           yyempty ? yyempty_ : yyla.type_get ()));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyempty)
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyempty = true;
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;
    yyerror_range[1].location = yystack_[yylen - 1].location;
    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yyerror_range[1].location = yystack_[0].location;
          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", error_token);
    }
    goto yynewstate;

    // Accept.
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    // Abort.
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (!yyempty)
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (!yyempty)
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  void
  BitTorrent_MetaInfo_Parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what());
  }

  // Generate an error message.
  std::string
  BitTorrent_MetaInfo_Parser::yysyntax_error_ (state_type yystate, symbol_number_type yytoken) const
  {
    std::string yyres;
    // Number of reported tokens (one for the "unexpected", one per
    // "expected").
    size_t yycount = 0;
    // Its maximum.
    enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
    // Arguments of yyformat.
    char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];

    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yytoken) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state
         merging (from LALR or IELR) and default reductions corrupt the
         expected token list.  However, the list is correct for
         canonical LR with one exception: it will still contain any
         token that will not be accepted due to an error action in a
         later state.
    */
    if (yytoken != yyempty_)
      {
        yyarg[yycount++] = yytname_[yytoken];
        int yyn = yypact_[yystate];
        if (!yy_pact_value_is_default_ (yyn))
          {
            /* Start YYX at -YYN if negative to avoid negative indexes in
               YYCHECK.  In other words, skip the first -YYN actions for
               this state because they are default actions.  */
            int yyxbegin = yyn < 0 ? -yyn : 0;
            // Stay within bounds of both yycheck and yytname.
            int yychecklim = yylast_ - yyn + 1;
            int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
            for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
              if (yycheck_[yyx + yyn] == yyx && yyx != yyterror_
                  && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
                {
                  if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                    {
                      yycount = 1;
                      break;
                    }
                  else
                    yyarg[yycount++] = yytname_[yyx];
                }
          }
      }

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
        YYCASE_(0, YY_("syntax error"));
        YYCASE_(1, YY_("syntax error, unexpected %s"));
        YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    // Argument number.
    size_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += yytnamerr_ (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char BitTorrent_MetaInfo_Parser::yypact_ninf_ = -5;

  const signed char BitTorrent_MetaInfo_Parser::yytable_ninf_ = -1;

  const signed char
  BitTorrent_MetaInfo_Parser::yypact_[] =
  {
      -2,    -1,     7,    -5,    -2,    -2,    -5,    -5,     5,    -5,
      -5,    -5,    -5,    -2,    -5,    -5,     5,    -5,     5,    -5,
      -5,    -5
  };

  const unsigned char
  BitTorrent_MetaInfo_Parser::yydefact_[] =
  {
      10,     0,     0,     2,    10,    10,    12,    13,     4,    11,
       1,     9,    15,    10,     5,     6,     4,    14,     4,     8,
       7,     3
  };

  const signed char
  BitTorrent_MetaInfo_Parser::yypgoto_[] =
  {
      -5,    -5,    -3,    -5,    -4,    -5,    -5
  };

  const signed char
  BitTorrent_MetaInfo_Parser::yydefgoto_[] =
  {
      -1,     2,    17,    18,     3,     4,     9
  };

  const unsigned char
  BitTorrent_MetaInfo_Parser::yytable_[] =
  {
      11,    12,     5,     1,     6,     7,     8,    10,    13,    19,
      14,    15,    16,    20,     0,    21
  };

  const signed char
  BitTorrent_MetaInfo_Parser::yycheck_[] =
  {
       4,     5,     3,     5,     5,     6,     7,     0,     3,    13,
       5,     6,     7,    16,    -1,    18
  };

  const unsigned char
  BitTorrent_MetaInfo_Parser::yystos_[] =
  {
       0,     5,     9,    12,    13,     3,     5,     6,     7,    14,
       0,    12,    12,     3,     5,     6,     7,    10,    11,    12,
      10,    10
  };

  const unsigned char
  BitTorrent_MetaInfo_Parser::yyr1_[] =
  {
       0,     8,     9,    10,    10,    11,    11,    11,    11,    12,
      12,    13,    14,    14,    14,    14
  };

  const unsigned char
  BitTorrent_MetaInfo_Parser::yyr2_[] =
  {
       0,     2,     1,     2,     0,     1,     1,     2,     2,     2,
       0,     2,     1,     1,     2,     2
  };



  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const BitTorrent_MetaInfo_Parser::yytname_[] =
  {
  "\"end\"", "error", "$undefined", "\"dictionary\"",
  "\"end_of_fragment\"", "\"string\"", "\"integer\"", "\"list\"",
  "$accept", "metainfo", "list_items", "list_item", "dictionary_items",
  "dictionary_item", "dictionary_value", YY_NULLPTR
  };

#if YYDEBUG
  const unsigned short int
  BitTorrent_MetaInfo_Parser::yyrline_[] =
  {
       0,   216,   216,   227,   228,   229,   239,   240,   249,   250,
     251,   252,   254,   284,   315,   339
  };

  // Print the state stack on the debug stream.
  void
  BitTorrent_MetaInfo_Parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  BitTorrent_MetaInfo_Parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):" << std::endl;
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG



} // yy




/* void
yy::BitTorrent_MetaInfo_Parser::error (const location_type& location_in,
                                       const std::string& message_in)
{
  NETWORK_TRACE (ACE_TEXT ("BitTorrent_MetaInfo_Parser::error"));

  try {
    iparser_p->error (location_in, message_in);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in BitTorrent_MetaInfo_IParser::error(), continuing\n")));
  }
}

void
yy::BitTorrent_MetaInfo_Parser::set (yyscan_t context_in)
{
  NETWORK_TRACE (ACE_TEXT ("BitTorrent_MetaInfo_Parser::set"));

  yyscanner = context_in;
} */

/*void
yysetdebug (int debug_in)
{
  NETWORK_TRACE (ACE_TEXT ("::yysetdebug"));

  yydebug = debug_in;
}*/

/*void
yyerror (YYLTYPE* location_in,
         BitTorrent_IParser* iparser_in,
         yyscan_t context_in,
         const char* message_in)
{
  NETWORK_TRACE (ACE_TEXT ("::yyerror"));

  ACE_UNUSED_ARG (location_in);
  ACE_UNUSED_ARG (context_in);

  // sanity check(s)
  ACE_ASSERT (iparser_in);

  try {
//    iparser_p->error (*location_in, message_in);
    iparser_p->error (message_in);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in BitTorrent_MetaInfo_IParser::error(), continuing\n")));
  }
}

void
yyprint (FILE* file_in,
         yytokentype type_in,
         YYSTYPE value_in)
{
  NETWORK_TRACE (ACE_TEXT ("::yyprint"));

  int result = -1;

  std::string format_string;
  switch (type_in)
  {
    case STRING:
    case INTEGER:
    case LIST:
    case DICTIONARY:
    {
      format_string = ACE_TEXT_ALWAYS_CHAR (" %s");
      break;
    }
    case END:
    {
      format_string = ACE_TEXT_ALWAYS_CHAR (" %d");
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown token type (was: %d), returning\n"),
                  type_in));
      return;
    }
  } // end SWITCH

  result = ACE_OS::fprintf (file_in,
                            ACE_TEXT (format_string.c_str ()),
                            value_in);
  if (result < 0)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fprintf(): \"%m\", returning\n")));
}*/
