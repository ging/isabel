/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */
/////////////////////////////////////////////////////////////////////////
//
// $Id: controlProtocol.h 6368 2005-03-28 10:27:14Z sirvent $
//
/////////////////////////////////////////////////////////////////////////

#ifndef _MCU_CTRL_PROTOCOL_H__
#define _MCU_CTRL_PROTOCOL_H__

#include <icf2/stdTask.hh>

class mcu_InterfaceDef_t
{
public:
    int interID;

    mcu_InterfaceDef_t(void) { interID= (int)this; }

    virtual ~mcu_InterfaceDef_t(void) {}

    //---------------COMMANDS-----------------------------//

    // session
    virtual char const *create_session        (int, char**);
    virtual char const *remove_session        (int, char**);

    // participants
    virtual char const *new_participant       (int, char**);
    virtual char const *remove_participant    (int, char**);
    virtual char const *configure_participant (int, char**);
    virtual char const *get_participants      (int, char**);
    virtual char const *bind_rtp              (int, char**);
    virtual char const *unbind_rtp            (int, char**);
    virtual char const *bind_rtcp             (int, char**);
    virtual char const *unbind_rtcp           (int, char**);

    // codec
    virtual char const *get_codecs            (int, char**);
    virtual char const *get_video_codecs      (int, char**);
    virtual char const *get_audio_codecs      (int, char**);

    // flows
    virtual char const *receive               (int, char**);
    virtual char const *discard               (int, char**);
    virtual char const *receive_video_mode    (int, char**);
    virtual char const *receive_video         (int, char**);
    virtual char const *discard_video         (int, char**);
    virtual char const *receive_audio_mode    (int, char**);
    virtual char const *receive_audio         (int, char**);
    virtual char const *discard_audio         (int, char**);

    // DTE interface (IF3 : MCU <-- DTE)
    virtual char const *get_audio_losses      (int, char**);
    virtual char const *get_video_losses      (int, char**);
    virtual char const *get_losses            (int, char**);
    virtual char const *configure_participant_audio (int, char**);
    virtual char const *configure_participant_video (int, char**);

    // system
    virtual char const *query_id              (int, char**);
    virtual char const *nop                   (int, char**);
    virtual char const *bye                   (int, char**);
    virtual char const *quit                  (int, char**);

    //----------------------------------------------------//
};

extern binding_t<mcu_InterfaceDef_t> mcu_methodBinding[];

typedef interface_t<mcu_InterfaceDef_t, mcu_methodBinding> mcu_Interface_t;

#endif

