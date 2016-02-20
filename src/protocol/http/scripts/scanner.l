%top{
#include "http_parser.h"

/*
#define YY_DECL                                                 \
yy::HTTP_Parser::token_type                                     \
HTTP_Scanner_lex (yy::HTTP_Parser::semantic_type* yylval_param, \
                  yy::HTTP_Parser::location_type* yylloc_param, \
                  HTTP_ParserDriver* driver,                    \
                  yyscan_t yyscanner) */
#define YY_DECL                              \
yytokentype                                  \
HTTP_Scanner_lex (YYSTYPE* yylval_param,     \
                  YYLTYPE* yylloc_param,     \
                  HTTP_ParserDriver* driver, \
                  yyscan_t yyscanner)
// ... and declare it for the parser's sake
YY_DECL;

//using namespace yy;
//#define YYLTYPE HTTP_Parser::location_type
//#define YYSTYPE HTTP_Parser::semantic_type
}

%{
#include <sstream>
#include <string>

#include "ace/ace_wchar.h"
#include "ace/Log_Msg.h"
#include "ace/Message_Block.h"
#include "ace/OS_Memory.h"

#include "net_macros.h"

#include "http_common.h"
#include "http_parser_driver.h"

/* *NOTE*: instead of the default (int), this yylex returns token_type.
           Unfortunately, yyterminate by default returns 0, which is not of
           token_type. This %define solves that issue. */
//#define yyterminate() return yy::HTTP_Parser::token::END
#define yyterminate() return yytokentype::END

/* *NOTE*: YY_DECL is defined elsewhere (needed so the scanner can support
           extra arguments) */
//#define YY_DECL
%}

%option header-file="http_scanner.h" outfile="http_scanner.cpp"
%option batch 8bit nodefault never-interactive nostdinit yylineno yywrap
/* %option c++ yyclass="HTTP_Scanner" */
/* %option ansi-definitions ansi-prototypes bison-bridge */
/* *NOTE*: the 'line' directives confuse gdb */
/* *NOTE*: 'noline' not supported (on Linux, flex 2.5.39)
           --> use --noline and (manually) remove '#line's introduced by %top */
%option ansi-definitions ansi-prototypes bison-locations
/* *IMPORTANT NOTE*: 'read' requires 'unistd'(.h) */
%option reentrant pointer prefix="HTTP_Scanner_" nomain
/* *TODO*: find out why 'read' does not compile (on Linux, flex 2.5.39) */
%option align read full
%option backup debug perf-report perf-report verbose warn
/* *IMPORTANT NOTE*: flex 2.5.4 does not recognize 'reentrant, nounistd,
                     ansi-definitions, ansi-prototypes, header-file extra-type'
*/
%option extra-type="HTTP_ParserDriver*"
/* %option nounput */

/* *NOTE*: see RFC 1945 page 10 */
OCTET          [\x00-\xFF]
/* *NOTE*: (US-)ASCII */
CHAR           [\x00-\x7F]
UPALPHA        [A-Z]
LOALPHA        [a-z]
ALPHA          [[:alpha:]]
DIGIT          [[:digit:]]
CTL            [\x00-\x1F\x7F]
CR             \x0D
LF             \x0A
SP             \x20
/* *NOTE*: (horizontal) TAB */
HT             \x09
DOUBLE_QUOTE   \x22


CRLF           \x0D\x0A
LWS            (\x0D\x0A(\x20|\x09){1})
/* *NOTE*: flex 2.5.4 does not support class unions ({+}) */
TEXT           ([\x00-\xFF]{-}[\x00-\x1F]{+}\x09\x0A\x0D])*
HEX            [a-fA-F[:digit:]]

T_SPECIALS     [()<>@,;:\\"/[\]?={} \x09]
TOKEN          [\x00-\x7F]{-}[\x00-\x1F\x7F]{+}[()<>@,;:\\"/[\]?={} \x09]

C_TEXT         ([\x00-\xFF]{-}[\x00-\x1F]{+}[\x09\x0A\x0D\x20]{-}[()])*
COMMENT        \x22([\x00-\xFF]{-}[\x00-\x1F]{+}[\x09\x0A\x0D\x20]{-}[()])*\x22

QD_TEXT        ([\x00-\xFF]{-}[\x00-\x1F\x22]{+}[\x09\x0A\x0D\x20])*
QUOTED_STRING  \x22([\x00-\xFF]{-}[\x00-\x1F\x22]{+}[\x09\x0A\x0D\x20])*\x22

WORD           ({TOKEN}|{QUOTED_STRING})

/* HTTP Version */
VERSION        HTTP\/[[:digit:]]{1}\.[[:digit:]]{1}


/* ESCAPE         (%(HEX)(HEX)) */

/* (UCHAR|:|@|&|=|+) */
P_CHAR         [[:alpha:][:digit:]$\-_.!*'(),%a-fA-F:@&=+]{+}[^[:alpha:][:digit:];/?:@&=+!*'(),$\-_.\x00-\x1F\x20\x22\x7F#%<>]
/* (UNRESERVED|ESCAPE) */
U_CHAR         [[:alpha:][:digit:]$\-_.!*'(),%a-fA-F]{+}[^[:alpha:][:digit:];/?:@&=+!*'(),$\-_.\x00-\x1F\x20\x22\x7F#%<>]
/* [[:alpha:][:digit:]]{+}(SAFE|EXTRA|NATIONAL) */
UNRESERVED     [[:alpha:][:digit:]$\-_.!*'(),]{+}[^[:alpha:][:digit:];/?:@&=+!*'(),$\-_.\x00-\x1F\x20\x22\x7F#%<>]

RESERVED       [;/?:@&=+]
EXTRA          [!*'(),]
SAFE           [$\-_.]
/* (CTL|SP|DOUBLEQUOTE){+}[#%<>] */
UNSAFE         [\x00-\x1F\x20\x22\x7F#%<>]
/* (OCTET){-}([[:alpha:][:digit:]]{+}(RESERVED|EXTRA|SAFE|UNSAFE)) */
NATIONAL       [^[:alpha:][:digit:];/?:@&=+!*'(),$\-_.\x00-\x1F\x20\x22\x7F#%<>]


/* Uniform Resource Identifiers */
NET_LOCATION   ({P_CHAR}|;|?)*
F_SEGMENT      {P_CHAR}{1}
SEGMENT        {P_CHAR}*
PATH           {F_SEGMENT}(\/{SEGMENT})*
PARAMETER      ({P_CHAR}|\/)*
PARAMETERS     {PARAMETER}(;{PARAMETER})*
QUERY          ({U_CHAR}|{RESERVED})*
/* *TODO*: specification is {PATH}{0,1}(;{PARAMETERS}){0,1}(?{QUERY_}){0,1}
           however, flex (2.5.39) does not compile */
RELATIVE_PATH  {PATH}{0,1}
ABSOLUTE_PATH  \/{RELATIVE_PATH}
NET_PATH       \/\/{NET_LOCATION}({ABSOLUTE_PATH}){0,1}
RELATIVE_URI   ({NET_PATH}|{ABSOLUTE_PATH}|{RELATIVE_PATH})
SCHEME         [[:alpha:][:digit:]+\-.]+
ABSOLUTE_URI   {SCHEME}:({U_CHAR}|{RESERVED})*
FRAGMENT       ({U_CHAR}|{RESERVED})*
URI            ({ABSOLUTE_URI}|{RELATIVE_URI})({FRAGMENT}(,{FRAGMENT})*)


/* http URL */
PORT           [[:digit:]]+
/* *NOTE*: "see RFC 952 [DNS:4] for details on allowed hostnames..."
RFC 952
1. A "name" (Net, Host, Gateway, or Domain name) is a text string up
   to 24 characters drawn from the alphabet (A-Z), digits (0-9), minus
   sign (-), and period (.).  Note that periods are only allowed when
   they serve to delimit components of "domain style names".
RFC 1123
The syntax of a legal Internet host name was specified in RFC-952.
One aspect of host name syntax is hereby changed: the restriction on
the first character is relaxed to allow either a letter or a digit.

And so on...
--> [[:alpha:][:digit:].-] is just a somewhat informed and tolerant suggestion.
    A more CORRECT implementation would probably look something like this...
<hostname>    = <label> | <hostname>.<label>
<label>       = <let-dig> | <let-dig><let-dig> | <let-dig><ldh-string><let-dig>
<ldh-string>  = <let-dig-hyp> | <ldh-string><let-dig-hyp>
<let-dig-hyp> = <let-dig> | '-'
<let-dig>     = <letter> | <digit>
<digit>       = [[:digit:]]
<letter>      = [[:alpha:]]*/
HOST           ([[:alpha:][:digit:].\-])+
URL            http://{HOST}(:{PORT}){0,1}{ABSOLUTE_PATH}


/* Date/Time Formats */
WKDAY          (Mon|Tue|Wed|Thu|Fri|Sat|Sun)
MONTH          (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)
DATE1          [[:digit:]]{2}{SP}{MONTH}{SP}[[:digit:]]{4}
TIME           [[:digit:]]{2}:[[:digit:]]{2}:[[:digit:]]{2}
RFC1123_DATE   {WKDAY},{SP}{DATE1}{SP}{TIME}{SP}GMT
WEEKDAY        (Monday|Tuesday|Wednesday|Thursday|Friday|Saturday|Sunday)
DATE2          [[:digit:]]{2}-{MONTH}-[[:digit:]]{2}
RFC850_DATE    {WEEKDAY},{SP}{DATE2}{SP}{TIME}{SP}GMT
DATE3          [[:digit:]]{2}-{MONTH}-[[:digit:]]{2}
ASCTIME_DATE   {WKDAY}{SP}{DATE3}{SP}{TIME}{SP}[[:digit:]]{4}
DATE           ({RFC1123_DATE}|{RFC850_DATE}|{ASCTIME_DATE})


/* Character Sets */
/* NOTE*: "...Applications should limit their use of character sets to those
          defined by the IANA registry." */
CHARSET_TOKEN  {CHAR}+
/* *NOTE*: "When no explicit charset parameter is provided by the sender, media
           subtypes of the "text" type are defined to have a default charset
           value of "ISO-8859-1" when received via HTTP"
CHARSET        (US-ASCII|ISO-8859-1|ISO-8859-2|ISO-8859-3|ISO-8859-4|ISO-8859-5|ISO-8859-6|ISO-8859-7|ISO-8859-8|ISO-8859-9|ISO-2022-JP|ISO-2022-JP-2|ISO-2022-KR|UNICODE-1-1|UNICODE-1-1-UTF-7|UNICODE-1-1-UTF-8|{CHARSET_TOKEN})


/* Content Coding */
CODING_TOKEN   {CHAR}+
CONTENT_CODING (x-gzip|x-compress|{CODING_TOKEN})


/* Media Types */
/* *NOTE*: "Media-type values are registered with the Internet Assigned Number
           Authority (IANA)" */
TYPE           {TOKEN}+
SUB_TYPE       {TOKEN}+
ATTRIBUTE      {TOKEN}+
VALUE          ({TOKEN}+|{QUOTED_STRING})
TYPE_PARAMETER {ATTRIBUTE}={VALUE}
MEDIA_TYPE     {TYPE}\/{SUB_TYPE}(;{TYPE_PARAMETER})*


/* Product Tokens */
PRODUCT_TOKEN   {TOKEN}+
PRODUCT_VERSION {PRODUCT_TOKEN}
PRODUCT         {PRODUCT_TOKEN}(\/{PRODUCT_VERSION}){0,1}


/* Message Headers (generic) */
FIELD_NAME      {TOKEN}+
/* ({TEXT}|[{TOKEN}{T_SPECIALS}{QUOTED_STRING}]*|{LWS})* */
FIELD_VALUE     {TOKEN}+
HEADER          {FIELD_NAME}:{SP}{FIELD_VALUE}

/* General Headers */
HEADER_DATE     Date:{SP}{DATE}
PRAGMA_TOKEN    {TOKEN}+(={WORD}){0,1}
PRAGMA_VALUE    (no-cache|{PRAGMA_TOKEN})
HEADER_PRAGMA   Pragma:{SP}{PRAGMA_VALUE}
GENERAL_HEADER  ({HEADER_DATE}|{HEADER_PRAGMA})

/* Request Headers */
/* *TODO*: see rfc1945 page 47 */
CREDENTIALS            {TOKEN}+
HEADER_AUTHORIZATION   Authorization:{SP}{CREDENTIALS}
/* *TODO*: e-mail address, see rfc822/1123 */
MAILBOX                {TOKEN}+
HEADER_FROM            From:{SP}{MAILBOX}
HEADER_IFMODIFIEDSINCE If-Modified-Since:{SP}{DATE}
HEADER_REFERER         Referer:{SP}(ABSOLUTE_URI|RELATIVE_URI)
HEADER_USERAGENT       User-Agent:{SP}({PRODUCT}|{COMMENT})
REQUEST_HEADER         ({HEADER_AUTHORIZATION}|{HEADER_FROM}|{HEADER_IFMODIFIEDSINCE}|{HEADER_REFERER}|{HEADER_USERAGENT})

/* Request-Line */
REQUEST_TOKEN   {CHAR}+
/* TODO*: allowing {REQUEST_TOKEN} breaks precedence in INITIAL state, as it
          gobbles the whole buffer */
/* METHOD          (GET|HEAD|POST|{REQUEST_TOKEN}) */
METHOD          (GET|HEAD|POST)
REQUEST_URI     ({ABSOLUTE_URI}|{ABSOLUTE_PATH})
REQUEST         {METHOD}{SP}{REQUEST_URI}{SP}{VERSION}

/* Response Headers */
LOCATION_HEADER        Location:{SP}{ABSOLUTE_URI}
SERVER_HEADER          Server:{SP}({PRODUCT}|{COMMENT})
AUTH_SCHEME            {TOKEN}+
REALM_VALUE            {QUOTED_STRING}
REALM                  realm={REALM_VALUE}
AUTH_PARAMETER         {TOKEN}+={QUOTED_STRING}
AUTH_PARAMETERS        (,{AUTH_PARAMETER})*
CHALLENGE              {AUTH_SCHEME}{SP}{REALM}{AUTH_PARAMETERS}
WWWAUTHENTICATE_HEADER WWW-Authenticate:{SP}{CHALLENGE}
RESPONSE_HEADER        ({LOCATION_HEADER}|{SERVER_HEADER}|{WWWAUTHENTICATE_HEADER})

/* Status-Line (Response-) */
/* ({TEXT}{-}{CRLF})* */
REASON          ([\x00-\xFF]{-}[\x00-\x1F]{+}[\x09])*
STATUS          {VERSION}{SP}[[:digit:]]{3}{SP}{REASON}

/* Entity Headers */
ALLOW_HEADER           Allow:{SP}{METHOD}+
CONTENTENCODING_HEADER Content-Encoding:{SP}{CONTENT_CODING}
CONTENTLENGTH_HEADER   Content-Length:{SP}[[:digit:]]+
CONTENTTYPE_HEADER     Content-Type:{SP}{MEDIA_TYPE}
EXPIRES_HEADER         Expires:{SP}{DATE}
LASTMODIFIED_HEADER    Last-Modified:{SP}{DATE}
EXTENSION_HEADER       {HEADER}
ENTITY_HEADER          ({ALLOW_HEADER}|{CONTENTENCODING_HEADER}|{CONTENTLENGTH_HEADER}|{CONTENTTYPE_HEADER}|{EXPIRES_HEADER}|{LASTMODIFIED_HEADER}|{EXTENSION_HEADER})

/* Chunks (*NOTE*: HTTP/1.1) */
CHUNKSIZE              {HEX}+
CHUNK_EXTENSION        {TOKEN}+(={TOKEN}+|{QUOTED_STRING}){0,1}
CHUNK_EXTENSIONS       (;{CHUNK_EXTENSION})*
CHUNK_LINE             {CHUNKSIZE}{CHUNK_EXTENSIONS}
CHUNK_DATA             {OCTET}+
/* CHUNK                  {CHUNK_LINE}{CRLF}{CHUNK_DATA}{CRLF} */
CHUNK_LINE_LAST        0{CHUNK_EXTENSIONS}
CHUNK_TRAILER          ({ENTITY_HEADER}{CRLF})*

%s             request
%s             method
%s             uri
%s             version
%s             request_headers
%s             response
%s             status
%s             reason
%s             response_headers
%s             body
%s             chunked_body
%s             chunk

%{
/* handle locations */
/*#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
                       yylloc.first_column = yycolumn; yylloc.last_column = yycolumn+yyleng-1; \
                       yycolumn += yyleng; */
#define YY_USER_ACTION yylloc->first_line = yylloc->last_line = yylineno; \
                       yylloc->first_column = yycolumn; yylloc->last_column = yycolumn+yyleng-1; \
                       yycolumn += yyleng;
//#define YY_USER_ACTION yylloc->columns (yyleng);
%}

%%
%{
  //yylloc->step ();
  yy_flex_debug = driver->getDebugScanner ();

  std::istringstream converter;
  unsigned int chunk_size;
%}

<INITIAL>{
{VERSION}                  {
                             BEGIN (response);
                             yycolumn -= yyleng;
                             /* yymore (); */ yyless (0); }
{METHOD}                   {
                             BEGIN (request);
                             yycolumn -= yyleng;
                             /* yymore (); */ yyless (0); }
} // end <INITIAL>
<request>{
{METHOD}{SP}               { driver->fragment_->rd_ptr (yyleng);
                             driver->offset_ += yyleng;
                             BEGIN (uri);
                             yylval->ival = yyleng;
                             /* *TODO*: error handling */
                             ACE_NEW_NORETURN (yylval->sval,
                                               std::string ());
                             ACE_ASSERT (yylval->sval);
                             yylval->sval->append (yytext, yyleng - 1);
                             return yytokentype::METHOD;
                             /* return yy::HTTP_Parser::token::METHOD; */ }
} // end <request>
<uri>{
{REQUEST_URI}{SP}          { driver->fragment_->rd_ptr (yyleng);
                             driver->offset_ += yyleng;
                             BEGIN (version);
                             yylval->ival = yyleng;
                             /* *TODO*: error handling */
                             ACE_NEW_NORETURN (yylval->sval,
                                               std::string ());
                             ACE_ASSERT (yylval->sval);
                             yylval->sval->append (yytext, yyleng - 1);
                             return yytokentype::URI;
                             /* return yy::HTTP_Parser::token::URI; */ }
} // end <uri>
<version>{
{VERSION}{CRLF}            { driver->fragment_->rd_ptr (yyleng);
                             driver->offset_ += yyleng;
                             BEGIN (request_headers);
                             yylval->ival = yyleng;
                             /* *TODO*: error handling */
                             ACE_NEW_NORETURN (yylval->sval,
                                               std::string ());
                             ACE_ASSERT (yylval->sval);
                             yylval->sval->append (yytext, yyleng - 2);
                             return yytokentype::VERSION;
                             /* return yy::HTTP_Parser::token::VERSION; */ }
} // end <version>
<request_headers>{
{GENERAL_HEADER}{CRLF}     { driver->fragment_->rd_ptr (yyleng);
                             driver->offset_ += yyleng;
                             yylval->ival = yyleng;
                             /* *TODO*: error handling */
                             ACE_NEW_NORETURN (yylval->sval,
                                               std::string ());
                             ACE_ASSERT (yylval->sval);
                             yylval->sval->append (yytext, yyleng - 2);
                             return yytokentype::HEADER;
                             /* return yy::HTTP_Parser::token::HEADER; */ }
{REQUEST_HEADER}{CRLF}     { driver->fragment_->rd_ptr (yyleng);
                             driver->offset_ += yyleng;
                             yylval->ival = yyleng;
                             /* *TODO*: error handling */
                             ACE_NEW_NORETURN (yylval->sval,
                                               std::string ());
                             ACE_ASSERT (yylval->sval);
                             yylval->sval->append (yytext, yyleng - 2);
                             return yytokentype::HEADER;
                             /* return yy::HTTP_Parser::token::HEADER; */ }
{ENTITY_HEADER}{CRLF}      { driver->fragment_->rd_ptr (yyleng);
                             driver->offset_ += yyleng;
                             yylval->ival = yyleng;
                             /* *TODO*: error handling */
                             ACE_NEW_NORETURN (yylval->sval,
                                               std::string ());
                             ACE_ASSERT (yylval->sval);
                             yylval->sval->append (yytext, yyleng - 2);
                             return yytokentype::HEADER;
                             /* return yy::HTTP_Parser::token::HEADER; */ }
{CRLF}                     { driver->fragment_->rd_ptr (yyleng);
                             driver->offset_ += yyleng;
                             BEGIN (body);
                             yylval->ival = 2;
                             return yytokentype::DELIMITER;
                             /* return yy::HTTP_Parser::token::DELIMITER; */ }
} // end <request_headers>
<response>{
{VERSION}{SP}              { driver->fragment_->rd_ptr (yyleng);
                             driver->offset_ += yyleng;
                             BEGIN (status);
                             yylval->ival = yyleng;
                             /* *TODO*: error handling */
                             ACE_NEW_NORETURN (yylval->sval,
                                               std::string ());
                             ACE_ASSERT (yylval->sval);
                             yylval->sval->append (yytext, yyleng - 1);
                             return yytokentype::VERSION;
                             /* return yy::HTTP_Parser::token::VERSION; */ }
} // end <response>
<status>{
[[:digit:]]{3}{SP}         { driver->fragment_->rd_ptr (yyleng);
                             driver->offset_ += yyleng;
                             BEGIN (reason);
                             yylval->ival = yyleng;
                             /* *TODO*: error handling */
                             ACE_NEW_NORETURN (yylval->sval,
                                               std::string ());
                             ACE_ASSERT (yylval->sval);
                             yylval->sval->append (yytext, yyleng - 1);
                             return yytokentype::STATUS;
                             /* return yy::HTTP_Parser::token::STATUS; */ }
} // end <status>
<reason>{
{REASON}{CRLF}             { driver->fragment_->rd_ptr (yyleng);
                             driver->offset_ += yyleng;
                             BEGIN (response_headers);
                             yylval->ival = yyleng;
                             /* *TODO*: error handling */
                             ACE_NEW_NORETURN (yylval->sval,
                                               std::string ());
                             ACE_ASSERT (yylval->sval);
                             yylval->sval->append (yytext, yyleng - 2);
                             return yytokentype::REASON;
                             /* return yy::HTTP_Parser::token::REASON; */ }
} // end <reason>
<response_headers>{
{GENERAL_HEADER}{CRLF}     { driver->fragment_->rd_ptr (yyleng);
                             driver->offset_ += yyleng;
                             yylval->ival = yyleng;
                             /* *TODO*: error handling */
                             ACE_NEW_NORETURN (yylval->sval,
                                               std::string ());
                             ACE_ASSERT (yylval->sval);
                             yylval->sval->append (yytext, yyleng - 2);
                             return yytokentype::HEADER;
                             /* return yy::HTTP_Parser::token::HEADER; */ }
{RESPONSE_HEADER}{CRLF}    { driver->fragment_->rd_ptr (yyleng);
                             driver->offset_ += yyleng;
                             yylval->ival = yyleng;
                             /* *TODO*: error handling */
                             ACE_NEW_NORETURN (yylval->sval,
                                               std::string ());
                             ACE_ASSERT (yylval->sval);
                             yylval->sval->append (yytext, yyleng - 2);
                             return yytokentype::HEADER;
                             /* return yy::HTTP_Parser::token::HEADER; */ }
{ENTITY_HEADER}{CRLF}      { driver->fragment_->rd_ptr (yyleng);
                             driver->offset_ += yyleng;
                             yylval->ival = yyleng;
                             /* *TODO*: error handling */
                             ACE_NEW_NORETURN (yylval->sval,
                                               std::string ());
                             ACE_ASSERT (yylval->sval);
                             yylval->sval->append (yytext, yyleng - 2);
                             return yytokentype::HEADER;
                             /* return yy::HTTP_Parser::token::HEADER; */ }
{CRLF}                     { driver->fragment_->rd_ptr (yyleng);
                             driver->offset_ += yyleng;
                             ACE_ASSERT (driver->record_);
                             HTTP_HeadersIterator_t iterator =
                                 driver->record_->headers.find (ACE_TEXT_ALWAYS_CHAR (HTTP_PRT_TRANSFER_ENCODING_HEADER_STRING));
                             if ((iterator != driver->record_->headers.end ()) &&
                                 ((*iterator).second == ACE_TEXT_ALWAYS_CHAR (HTTP_PRT_TRANSFER_ENCODING_CHUNKED_STRING)))
                             {
//                               ACE_DEBUG ((LM_DEBUG,
//                                           ACE_TEXT ("entity uses chunked transfer...\n")));
                               BEGIN (chunked_body);
                             } // end IF
                             else
                               BEGIN (body);
                             yylval->ival = 2;
                             return yytokentype::DELIMITER;
                             /* return yy::HTTP_Parser::token::DELIMITER; */ }
} // end <response_headers>
<body>{
{OCTET}*                   { driver->offset_ += yyleng;
//                             ACE_DEBUG ((LM_DEBUG,
//                                         ACE_TEXT ("found entity @%d[%d]: %d byte(s)\n"),
//                                         driver->offset_,
//                                         (driver->fragment_->rd_ptr () - driver->fragment_->base ()),
//                                         driver->fragment_->length ()));
                             yylval->ival = yyleng;
                             //YY_FLUSH_BUFFER; // --> use the next fragment
                             return yytokentype::BODY;
                             /* return yy::HTTP_Parser::token::BODY; */ }
} // end <body>
<chunked_body>{
{CHUNKSIZE}                {
                             converter.setf (std::ios::hex,
                                             std::ios::basefield);
                             converter.str (yytext);
                             converter >> chunk_size;
                             converter.str (ACE_TEXT_ALWAYS_CHAR (""));
                             converter.clear ();
                             BEGIN (chunk);
                             yycolumn -= yyleng;
                             /* yymore (); */ yyless (0); }
{ENTITY_HEADER}{CRLF}      { driver->fragment_->rd_ptr (yyleng);
                             driver->offset_ += yyleng;
                             yylval->ival = yyleng;
                             /* *TODO*: error handling */
                             ACE_NEW_NORETURN (yylval->sval,
                                               std::string ());
                             ACE_ASSERT (yylval->sval);
                             yylval->sval->append (yytext, yyleng - 2);
                             return yytokentype::HEADER;
                             /* return yy::HTTP_Parser::token::HEADER; */ }
{CRLF}                     { driver->fragment_->rd_ptr (yyleng);
                             driver->offset_ += yyleng;
                             driver->finished_ = true; // done
                             BEGIN (INITIAL);
                             yylval->ival = 2;
                             return yytokentype::DELIMITER;
                             /* return yy::HTTP_Parser::token::DELIMITER; */ }
} // end <chunked_body>
<chunk>{
{CHUNK_LINE}{CRLF}         { driver->fragment_->rd_ptr (yyleng);
                             driver->offset_ += yyleng;
                             if (chunk_size)
                             {
                               /* undo the effects of YY_DO_BEFORE_ACTION */
                               *yy_cp = yyg->yy_hold_char;

                               // NOT the final chunk
                               // --> there is (trailing) entity data
//                               ACE_DEBUG ((LM_DEBUG,
//                                           ACE_TEXT ("found entity chunk @%d[%d]: %d + %d byte(s)\n"),
//                                           driver->offset_ - yyleng,
//                                           (driver->fragment_->rd_ptr () - driver->fragment_->base ()) - yyleng,
//                                           yyleng, chunk_size));

                               // adjust write pointer ?
                               ACE_Message_Block* message_p, *message_2;
                               unsigned int remainder =
                                   driver->fragment_->length ();
                               if (chunk_size <= remainder)
                               { // current fragment contains the whole chunk

                                 // --> insert buffer
                                 message_p =
                                   driver->fragment_->duplicate ();
                                 ACE_ASSERT (message_p);
                                 message_2 =
                                   driver->fragment_->cont ();
                                 if (message_2)
                                   message_p->cont (message_2);
                                 driver->fragment_->cont (message_p);

                                 driver->fragment_->wr_ptr (driver->fragment_->rd_ptr () + chunk_size);
                                 remainder = chunk_size;
                               } // end IF
                               else
                               {
                                 // skip trailing entity data fragments
                                 unsigned int skipped_bytes = remainder;
                                 while (skipped_bytes <= chunk_size)
                                 {
                                   if (!driver->switchBuffer ())
                                   {
                                     ACE_DEBUG ((LM_ERROR,
                                                 ACE_TEXT ("failed to HTTP_ParserDriver::switchBuffer(), aborting\n")));
                                     yyterminate();
                                   } // end IF

                                   skipped_bytes +=
                                       driver->fragment_->length ();
                                 } // end WHILE
                                 remainder = (skipped_bytes - chunk_size);

                                 // this chunk ends in the current fragment
                                 // --> insert buffer, adjust writer pointer,
                                 //     switch buffers one more time
                                 message_p =
                                     driver->fragment_->duplicate ();
                                 ACE_ASSERT (message_p);
                                 message_2 =
                                     driver->fragment_->cont ();
                                 if (message_2)
                                   message_p->cont (message_2);
                                 driver->fragment_->cont (message_p);
                                 // computer offset to end of chunk
                                 remainder =
                                     driver->fragment_->length () - remainder;
                                 driver->fragment_->wr_ptr (driver->fragment_->rd_ptr () + remainder);
                               } // end ELSE

                               if (!driver->switchBuffer ())
                               {
                                 ACE_DEBUG ((LM_ERROR,
                                             ACE_TEXT ("failed to HTTP_ParserDriver::switchBuffer(), aborting\n")));
                                 yyterminate();
                               } // end IF
                               remainder += 2;
                               driver->fragment_->rd_ptr (remainder); // offset + CRLF

                               // gobble initial bytes
                               yyg->yy_c_buf_p += remainder;
                               yyg->yy_hold_char = *yyg->yy_c_buf_p;
                               //YY_FLUSH_BUFFER; // --> refill scan buffer
//                               char c;
//                               for (unsigned int i = 0;
//                                    i < remainder;
//                                    ++i)
//                                 c = yyinput (yyscanner);
//                               ACE_UNUSED_ARG (c);

                               driver->offset_ += (chunk_size + 2);
                               yylval->ival = yyleng + (chunk_size + 2);
                             } // end IF
                             else
                             {
//                               ACE_DEBUG ((LM_DEBUG,
//                                           ACE_TEXT ("found last entity chunk @%d[%d]\n"),
//                                           driver->offset_ - yyleng,
//                                           (driver->fragment_->rd_ptr () - driver->fragment_->base ()) - yyleng));
                               yylval->ival = yyleng;
                             } // end ELSE
                             BEGIN (chunked_body);
                             return yytokentype::CHUNK;
                             /* return yy::HTTP_Parser::token::CHUNK; */ }
} // end <chunk>
<<EOF>>                    { yyterminate(); }
<*>{OCTET}                 { /* *TODO*: use (?s:.) ? */

                             /* *NOTE*: should not be reached, unless something
                                        went terribly wrong */
                             ACE_ASSERT (false);

                             // debug info
                             std::string error_message =
                               ACE_TEXT_ALWAYS_CHAR ("invalid character: \"");
                             error_message += yytext[0];
                             error_message += ACE_TEXT_ALWAYS_CHAR ("\", aborting");
                             driver->error (*yylloc, error_message);
                             yyterminate(); }
%%
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
int
HTTP_Scanner_wrap (yyscan_t yyscanner)
{
  NETWORK_TRACE (ACE_TEXT ("::HTTP_Scanner_wrap"));

  struct yyguts_t* yyg = static_cast<struct yyguts_t*> (yyscanner);
  HTTP_ParserDriver* driver = HTTP_Scanner_get_extra (yyscanner);

  // sanity check(s)
  ACE_ASSERT (driver);
  if (driver->finished_)
    return 1; // done
  ACE_ASSERT (driver->fragment_);

  // *NOTE*: there is more data
  // 1. gobble/save the rest
  // 2. switch buffers
  // 3. unput the rest
  // 4. continue scanning

  // step1
  std::string the_rest;
  the_rest.append (yytext, yyleng);
//  for (char c = yyinput (yyscanner);
//       c != EOF;
//       c = yyinput (yyscanner));
  yyg->yy_c_buf_p += yyleng;
  yyg->yy_hold_char = *yyg->yy_c_buf_p;
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("the rest: \"%s\"\n"),
              ACE_TEXT (the_rest.c_str ())));

  // step2
  if (!driver->switchBuffer ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to HTTP_ParserDriver::switchBuffer() [remaining data was: \"%s\"], aborting\n"),
                ACE_TEXT (the_rest.c_str ())));
    return 1;
  } // end IF

  // step3
  for (std::string::reverse_iterator iterator = the_rest.rbegin ();
       iterator != the_rest.rend ();
       ++iterator)
    unput (*iterator);

  // step4
  // yymore ();
  return 0;
}
#ifdef __cplusplus
}
#endif /* __cplusplus */