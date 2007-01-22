
#ifndef _AGENT_H
#define _AGENT_H

typedef enum media_type MediaType;

enum media_type
{
  MEDIA_TYPE_AUDIO,
  MEDIA_TYPE_VIDEO,
};


/*** address ***/


typedef enum address_type AddressType;

enum address_type
{
  ADDRESS_TYPE_IPV4,
  ADDRESS_TYPE_IPV6,
};

typedef struct _address Address;

/* XXX: need access to fields to convert to sockaddr_in */
struct _address
{
  AddressType type;
  union
  {
    guint32 addr_ipv4;
    guchar addr_ipv6[16];
  };
};

Address *
address_new_ipv4 (guint32 addr_ipv4);
Address *
address_new_ipv4_from_string (gchar *str);
gboolean
address_equal (Address *a, Address *b);
gchar *
address_to_string (Address *addr);
void
address_free (Address *addr);


/*** candidate ***/

typedef enum candidate_type CandidateType;

enum candidate_type
{
  CANDIDATE_TYPE_HOST,
  CANDIDATE_TYPE_SERVER_REFLEXIVE,
  CANDIDATE_TYPE_PEER_REFLEXIVE,
  CANDIDATE_TYPE_RELAYED,
};


typedef struct _candidate Candidate;

struct _candidate
{
  CandidateType type;
  guint id;
  Address *addr;
  Address *base_addr;
  // guint sock;
  guint16 port;
  guint32 priority;
  // Stream *stream;
  guint stream_id;
  guint component_id;
  // guint generation;
  // gchar *foundation;
  UDPSocket sock;
};


Candidate *
candidate_new (CandidateType type);
void
candidate_free (Candidate *candidate);


/*** event ***/


typedef enum event_type EventType;

enum event_type
{
  EVENT_CANDIDATE_SELECTED,
};


typedef struct _event Event;

struct _event
{
  EventType type;

  union {
    struct {
      Address *addr;
      guint candidate_id;
    } request_port;
    struct {
      Address *from;
      guint from_port;
      Address *to;
      guint to_port;
    } request_stun_query;
  };
};


void
event_free (Event *ev);


/*** agent ***/


typedef struct _agent Agent;

struct _agent
{
  guint next_candidate_id;
  guint next_stream_id;
  UDPSocketManager *sockmgr;
  GSList *local_addresses;
  GSList *local_candidates;
  GSList *remote_candidates;
  GSList *streams;
  GSList *events;
};


Agent *
ice_agent_new (UDPSocketManager *mgr);
Event *
ice_agent_pop_event (Agent *agent);
void
ice_agent_add_local_address (Agent *agent, Address *addr);
guint
ice_agent_add_stream (
  Agent *agent,
  MediaType type,
  void (*handle_recv) (Agent *agent, guint stream_id, guint len, gchar *buf));
void
ice_agent_free (Agent *agent);
void
ice_agent_add_remote_candidate (
  Agent *agent,
  CandidateType type,
  Address *addr,
  guint port);
void
ice_agent_recv (
  Agent *agent,
  guint candidate_id);

#endif /* _AGENT_H */

