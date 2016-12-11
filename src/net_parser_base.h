/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns   *
 *   erik.sohns@web.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef NET_PARSER_BASE_T_H
#define NET_PARSER_BASE_T_H

#include <iostream>
#include <string>

#include <ace/Global_Macros.h>

#include "common_iinitialize.h"

#include "net_defines.h"
#include "net_iparser.h"

// forward declaration(s)
struct yy_buffer_state;
class ACE_Message_Block;
class ACE_Message_Queue_Base;
typedef void* yyscan_t;

template <typename ConfigurationType,
          typename ScannerType, // (f/)lex-
          typename ParserType, // yacc/bison-
          typename ParserInterfaceType, // derived from Net_IParser
          typename ArgumentType, // yacc/bison-
          typename SessionMessageType>
class Net_CppParserBase_T
 : public ParserInterfaceType
{
 public:
  Net_CppParserBase_T (bool,  // debug scanning ?
                       bool); // debug parsing ?
  virtual ~Net_CppParserBase_T ();

  // implement (part of) ParserInterfaceType
  virtual bool initialize (const ConfigurationType&);
  inline virtual ACE_Message_Block* buffer () { return fragment_; };
//  inline virtual bool debugScanner () const { return bittorrent_get_debug (scannerState_); };
  inline virtual bool debugScanner () const { ACE_ASSERT (false); ACE_NOTSUP_RETURN (false); ACE_NOTREACHED (return false;) };
  inline virtual bool isBlocking () const { return blockInParse_; };
//  virtual void error (const YYLTYPE&,      // location
//                      const std::string&); // message
  virtual void error (const std::string&); // message
  inline virtual void offset (unsigned int offset_in) { offset_ += offset_in; }; // offset (increment)
  inline virtual unsigned int offset () const { return offset_; };
  virtual bool parse (ACE_Message_Block*); // data buffer handle
  virtual bool switchBuffer (bool = false); // unlink current fragment ?
  // *NOTE*: (waits for and) appends the next data chunk to fragment_;
  virtual void wait ();

  inline virtual void dump_state () const { ACE_ASSERT (false); ACE_NOTSUP; };

 protected:
  ConfigurationType*      configuration_;
  ACE_Message_Block*      fragment_;
  unsigned int            offset_; // parsed fragment bytes
  bool                    trace_;

  // parser
  ParserType              parser_;
//  ArgumentType            argument_;

  // scanner
  ScannerType             scanner_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Net_CppParserBase_T ())
  ACE_UNIMPLEMENTED_FUNC (Net_CppParserBase_T (const Net_CppParserBase_T&))
  ACE_UNIMPLEMENTED_FUNC (Net_CppParserBase_T& operator= (const Net_CppParserBase_T&))

  // helper methods
  bool scan_begin ();
  void scan_end ();

  // helper types
  struct MEMORY_BUFFER_T
   : std::streambuf
  {
    void set (char* buffer_in, unsigned int size_in) {
      this->setg (buffer_in, buffer_in, buffer_in + size_in);
    }
  };

  bool                    blockInParse_;
  bool                    isFirst_;

  struct yy_buffer_state* buffer_;
  MEMORY_BUFFER_T         streamBuffer_;
  std::istream            stream_;

  ACE_Message_Queue_Base* messageQueue_;

  bool                    isInitialized_;
};

//////////////////////////////////////////

template <typename ConfigurationType,
          typename ParserType, // yacc/bison-
          typename ParserInterfaceType, // derived from Net_IParser
          typename ArgumentType, // yacc/bison-
          typename SessionMessageType>
class Net_ParserBase_T
 : public ParserInterfaceType
 , public Net_IScanner_T<ParserInterfaceType>
{
 public:
  Net_ParserBase_T (bool,  // debug scanning ?
                    bool); // debug parsing ?
  virtual ~Net_ParserBase_T ();

  // implement (part of) ParserInterfaceType
  virtual bool initialize (const ConfigurationType&);
  inline virtual ACE_Message_Block* buffer () { return fragment_; };
//  inline virtual bool debugScanner () const { return bittorrent_get_debug (scannerState_); };
  inline virtual bool debugScanner () const { ACE_ASSERT (false); ACE_NOTSUP_RETURN (false); ACE_NOTREACHED (return false;) };
  inline virtual bool isBlocking () const { return blockInParse_; };
//  virtual void error (const YYLTYPE&,      // location
//                      const std::string&); // message
  virtual void error (const std::string&); // message
  inline virtual void offset (unsigned int offset_in) { offset_ += offset_in; }; // offset (increment)
  inline virtual unsigned int offset () const { return offset_; };
  virtual bool parse (ACE_Message_Block*); // data buffer handle
  virtual bool switchBuffer (bool = false); // unlink current fragment ?
  // *NOTE*: (waits for and) appends the next data chunk to fragment_;
  virtual void wait ();

  inline virtual void dump_state () const { ACE_ASSERT (false); ACE_NOTSUP; };

 protected:
  // implement (part of) Net_IScanner_T
  inline virtual void debug (yyscan_t,
                             bool) { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) };
  inline virtual bool initialize (yyscan_t&) { ACE_ASSERT (false); ACE_NOTSUP_RETURN (false); ACE_NOTREACHED (return false;) };
  inline virtual void finalize (yyscan_t&) { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) };
  inline virtual void destroy (yyscan_t,
                               struct yy_buffer_state*&) { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) };

  ConfigurationType*      configuration_;
  ACE_Message_Block*      fragment_;
  unsigned int            offset_; // parsed fragment bytes
  bool                    trace_;

  // parser
  ParserType              parser_;
//  ArgumentType            argument_;

  // scanner
  yyscan_t                state_;
  bool                    useYYScanBuffer_;

 private:
  ACE_UNIMPLEMENTED_FUNC (Net_ParserBase_T ())
  ACE_UNIMPLEMENTED_FUNC (Net_ParserBase_T (const Net_ParserBase_T&))
  ACE_UNIMPLEMENTED_FUNC (Net_ParserBase_T& operator= (const Net_ParserBase_T&))

  // convenient types
//  typedef Net_IScanner_T<ParserInterfaceType> ISCANNER_T;

  // helper methods
  bool scan_begin ();
  void scan_end ();

  bool                    blockInParse_;
  bool                    isFirst_;

  struct yy_buffer_state* buffer_;
  ACE_Message_Queue_Base* messageQueue_;

  bool                    isInitialized_;
};

// include template definition
#include "net_parser_base.inl"

#endif
