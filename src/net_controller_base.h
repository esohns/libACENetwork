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

#ifndef NET_CONTROLLER_BASE_H
#define NET_CONTROLLER_BASE_H

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_itask.h"

class Net_Controller_Base
 : public Common_ITask_T<ACE_MT_SYNCH,
                         Common_ILock_T<ACE_MT_SYNCH> >
{
  typedef Common_ITask_T<ACE_MT_SYNCH,
                         Common_ILock_T<ACE_MT_SYNCH> > inherited;

 public:
  Net_Controller_Base ();
  virtual ~Net_Controller_Base ();

  // implement Common_ITask_T
  virtual void start ();
  virtual void stop (bool = true,  // wait for completion ?
                     bool = true); // locked access ?
  virtual bool isRunning () const;

 private:
  ACE_UNIMPLEMENTED_FUNC (Net_Controller_Base (const Net_Controller_Base&));
  ACE_UNIMPLEMENTED_FUNC (Net_Controller_Base& operator= (const Net_Controller_Base&));
};

#endif
