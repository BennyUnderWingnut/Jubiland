/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: response.proto */

#ifndef PROTOBUF_C_response_2eproto__INCLUDED
#define PROTOBUF_C_response_2eproto__INCLUDED

#include <protobuf-c/protobuf-c.h>

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1000000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1003001 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif


typedef struct _WelcomeMessage WelcomeMessage;
typedef struct _Response Response;


/* --- enums --- */

typedef enum _Response__RequestType {
  RESPONSE__REQUEST_TYPE__WELCOME_MESSAGE = 0,
  RESPONSE__REQUEST_TYPE__LOGIN_FAIL = 1
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(RESPONSE__REQUEST_TYPE)
} Response__RequestType;

/* --- messages --- */

struct  _WelcomeMessage
{
  ProtobufCMessage base;
  uint64_t id;
  int32_t key;
};
#define WELCOME_MESSAGE__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&welcome_message__descriptor) \
    , 0, 0 }


struct  _Response
{
  ProtobufCMessage base;
  Response__RequestType type;
  WelcomeMessage *welcomemsg;
};
#define RESPONSE__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&response__descriptor) \
    , RESPONSE__REQUEST_TYPE__WELCOME_MESSAGE, NULL }


/* WelcomeMessage methods */
void   welcome_message__init
                     (WelcomeMessage         *message);
size_t welcome_message__get_packed_size
                     (const WelcomeMessage   *message);
size_t welcome_message__pack
                     (const WelcomeMessage   *message,
                      uint8_t             *out);
size_t welcome_message__pack_to_buffer
                     (const WelcomeMessage   *message,
                      ProtobufCBuffer     *buffer);
WelcomeMessage *
       welcome_message__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   welcome_message__free_unpacked
                     (WelcomeMessage *message,
                      ProtobufCAllocator *allocator);
/* Response methods */
void   response__init
                     (Response         *message);
size_t response__get_packed_size
                     (const Response   *message);
size_t response__pack
                     (const Response   *message,
                      uint8_t             *out);
size_t response__pack_to_buffer
                     (const Response   *message,
                      ProtobufCBuffer     *buffer);
Response *
       response__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   response__free_unpacked
                     (Response *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*WelcomeMessage_Closure)
                 (const WelcomeMessage *message,
                  void *closure_data);
typedef void (*Response_Closure)
                 (const Response *message,
                  void *closure_data);

/* --- services --- */


/* --- descriptors --- */

extern const ProtobufCMessageDescriptor welcome_message__descriptor;
extern const ProtobufCMessageDescriptor response__descriptor;
extern const ProtobufCEnumDescriptor    response__request_type__descriptor;

PROTOBUF_C__END_DECLS


#endif  /* PROTOBUF_C_response_2eproto__INCLUDED */
