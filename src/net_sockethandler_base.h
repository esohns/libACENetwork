/***************************************************************************
 *   Copyright (C) 2010 by Erik Sohns   *
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

#ifndef Net_SOCKETHANDLER_BASE_H
#define Net_SOCKETHANDLER_BASE_H

#include "ace/Global_Macros.h"

#include "common_iinitialize.h"

//#include "net_exports.h"

template <typename ConfigurationType/*,
          typename SessionDataType*/>
class Net_SocketHandlerBase
 : public Common_IInitialize_T<ConfigurationType>
{
 public:
  virtual ~Net_SocketHandlerBase ();

  // implement Common_IInitialize_T
  virtual bool initialize (const ConfigurationType&); // handler configuration

 protected:
  Net_SocketHandlerBase ();

  ConfigurationType configuration_;
  bool              isInitialized_;
  //SessionDataType   sessionData_;

 private:
//  ACE_UNIMPLEMENTED_FUNC (Net_SocketHandlerBase ());
  ACE_UNIMPLEMENTED_FUNC (Net_SocketHandlerBase (const Net_SocketHandlerBase&));
  ACE_UNIMPLEMENTED_FUNC (Net_SocketHandlerBase& operator= (const Net_SocketHandlerBase&));
};

#include "net_sockethandler_base.inl"

#endif
