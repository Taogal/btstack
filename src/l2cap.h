/*
 * Copyright (C) 2009 by Matthias Ringwald
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MATTHIAS RINGWALD AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MATTHIAS
 * RINGWALD OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/*
 *  l2cap.h
 *
 *  Logical Link Control and Adaption Protocl (L2CAP)
 *
 *  Created by Matthias Ringwald on 5/16/09.
 */

#pragma once

#include "hci.h"
#include "l2cap_signaling.h"
#include <btstack/utils.h>
#include <btstack/btstack.h>

#if defined __cplusplus
extern "C" {
#endif
    
#define L2CAP_SIG_ID_INVALID 0

#define L2CAP_HEADER_SIZE 4

void l2cap_init(void);
void l2cap_register_packet_handler(void (*handler)(void * connection, uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size));
void l2cap_create_channel_internal(void * connection, btstack_packet_handler_t packet_handler, bd_addr_t address, uint16_t psm, uint16_t mtu);
void l2cap_disconnect_internal(uint16_t local_cid, uint8_t reason);
int l2cap_send_internal(uint16_t local_cid, uint8_t *data, uint16_t len);
uint16_t l2cap_get_remote_mtu_for_local_cid(uint16_t local_cid);

void l2cap_block_new_credits(uint8_t blocked);
int  l2cap_can_send_packet_now(uint16_t local_cid);    // non-blocking UART write

// get outgoing buffer and prepare data
uint8_t *l2cap_get_outgoing_buffer(void);
int l2cap_send_prepared(uint16_t local_cid, uint16_t len);

    
void l2cap_close_connection(void *connection);

void l2cap_register_service_internal(void *connection, btstack_packet_handler_t packet_handler, uint16_t psm, uint16_t mtu);
void l2cap_unregister_service_internal(void *connection, uint16_t psm);

void l2cap_accept_connection_internal(uint16_t local_cid);
void l2cap_decline_connection_internal(uint16_t local_cid, uint8_t reason);


// private structs
typedef enum {
    L2CAP_STATE_CLOSED = 1,           // no baseband
    L2CAP_STATE_WILL_SEND_CREATE_CONNECTION,
    L2CAP_STATE_WAIT_CONNECTION_COMPLETE,
    L2CAP_STATE_WAIT_CLIENT_ACCEPT_OR_REJECT,
    L2CAP_STATE_WAIT_CONNECT_RSP, // from peer
    L2CAP_STATE_CONFIG,
    L2CAP_STATE_OPEN,
    L2CAP_STATE_WAIT_DISCONNECT,  // from application
    L2CAP_STATE_WILL_SEND_CONNECTION_REQUEST,
    L2CAP_STATE_WILL_SEND_CONNECTION_RESPONSE_DECLINE,
    L2CAP_STATE_WILL_SEND_CONNECTION_RESPONSE_ACCEPT,   
    L2CAP_STATE_WILL_SEND_DISCONNECT_REQUEST,
    L2CAP_STATE_WILL_SEND_DISCONNECT_RESPONSE,
} L2CAP_STATE;

typedef enum {
    L2CAP_CHANNEL_STATE_VAR_NONE          = 0,
    L2CAP_CHANNEL_STATE_VAR_RCVD_CONF_REQ = 1 << 0,
    L2CAP_CHANNEL_STATE_VAR_RCVD_CONF_RSP = 1 << 1,
    L2CAP_CHANNEL_STATE_VAR_SEND_CONF_REQ = 1 << 2,
    L2CAP_CHANNEL_STATE_VAR_SEND_CONF_RSP = 1 << 3,
    L2CAP_CHANNEL_STATE_VAR_SENT_CONF_REQ = 1 << 4,
    L2CAP_CHANNEL_STATE_VAR_SENT_CONF_RSP = 1 << 5,
} L2CAP_CHANNEL_STATE_VAR;

// info regarding an actual coneection
typedef struct {
    // linked list - assert: first field
    linked_item_t    item;
    
    L2CAP_STATE state;
    L2CAP_CHANNEL_STATE_VAR state_var;
    
    bd_addr_t address;
    hci_con_handle_t handle;
    
    uint8_t   remote_sig_id;    // used by other side, needed for delayed response
    uint8_t   local_sig_id;     // own signaling identifier
    
    uint16_t  local_cid;
    uint16_t  remote_cid;
    
    uint16_t  local_mtu;
    uint16_t  remote_mtu;
    
    uint16_t  psm;
    
    uint8_t   packets_granted;    // number of L2CAP/ACL packets client is allowed to send
    
    uint8_t   reason; // used in decline internal
    
    // client connection
    void * connection;
    
    // internal connection
    btstack_packet_handler_t packet_handler;
    
} l2cap_channel_t;

// info regarding potential connections
typedef struct {
    // linked list - assert: first field
    linked_item_t    item;
    
    // service id
    uint16_t  psm;
    
    // incoming MTU
    uint16_t mtu;
    
    // client connection
    void *connection;    
    
    // internal connection
    btstack_packet_handler_t packet_handler;
    
} l2cap_service_t;


typedef struct l2cap_signaling_response {
    hci_con_handle_t handle;
    uint8_t  sig_id;
    uint8_t  code;
    uint16_t data; // infoType for INFORMATION REQUEST, result for CONNECTION request
} l2cap_signaling_response_t;
    
    
#if defined __cplusplus
}
#endif
