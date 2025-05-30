#if !defined(HTTPHEADER_H)
#define HTTPHEADER_H

#include <iostream>
#include <map>

namespace rs::block
{
enum class HttpHeaderEnum
{
  Accept = 0,
  AcceptCharset = 1,
  AcceptEncoding = 2,
  AcceptLanguage = 3,
  AcceptRanges = 4,
  Age = 5,
  Allow = 6,
  Authorization = 7,
  CacheControl = 8,
  Connection = 9,
  ContentEncoding = 10,
  ContentLanguage = 11,
  ContentLength = 12,
  ContentLocation = 13,
  ContentMD5 = 14,
  ContentRange = 15,
  ContentType = 16,
  Cookie = 17,
  Date = 18,
  ETag = 19,
  Expect = 20,
  Expires = 21,
  From = 22,
  Host = 23,
  IfMatch = 24,
  IfModifiedSince = 25,
  IfNoneMatch = 26,
  IfRange = 27,
  IfUnmodifiedSince = 28,
  LastModified = 29,
  Location = 30,
  MaxForwards = 31,
  Pragma = 32,
  ProxyAuthenticate = 33,
  ProxyAuthorization = 34,
  Range = 35,
  Referer = 36,
  RetryAfter = 37,
  Server = 38,
  SetCookie = 39,
  TE = 40,
  Trailer = 41,
  TransferEncoding = 42,
  Upgrade = 43,
  UserAgent = 44,
  Vary = 45,
  Via = 46,
  Warning = 47,
  WWWAuthenticate = 48,
  Custom = 49,
  NoHeader
};

enum class HttpRequestTypeEnum
{
  Get = 0,
  Post = 1,
  Put = 2,
  Delete = 3,
  Head = 4,
  Options = 5,
  Patch = 6,
  Connect = 7,
  Trace = 8,
  NoRequest,
};

enum class HttpStatusEnum
{
  Continue = 100,
  SwitchingProtocols = 101,
  Processing = 102,
  EarlyHints = 103,

  OK = 200,
  Created = 201,
  Accepted = 202,
  NonAuthoritativeInformation = 203,
  NoContent = 204,
  ResetContent = 205,
  PartialContent = 206,
  MultiStatus = 207,
  AlreadyReported = 208,
  IMUsed = 226,

  MultipleChoices = 300,
  MovedPermanently = 301,
  Found = 302,
  SeeOther = 303,
  NotModified = 304,
  UseProxy = 305,
  TemporaryRedirect = 307,
  PermanentRedirect = 308,

  BadRequest = 400,
  Unauthorized = 401,
  PaymentRequired = 402,
  Forbidden = 403,
  NotFound = 404,
  MethodNotAllowed = 405,
  NotAcceptable = 406,
  ProxyAuthenticationRequired = 407,
  RequestTimeout = 408,
  Conflict = 409,
  Gone = 410,
  LengthRequired = 411,
  PreconditionFailed = 412,
  PayloadTooLarge = 413,
  URITooLong = 414,
  UnsupportedMediaType = 415,
  RangeNotSatisfiable = 416,
  ExpectationFailed = 417,
  ImATeapot = 418,
  MisdirectedRequest = 421,
  UnprocessableEntity = 422,
  Locked = 423,
  FailedDependency = 424,
  TooEarly = 425,
  UpgradeRequired = 426,
  PreconditionRequired = 428,
  TooManyRequests = 429,
  RequestHeaderFieldsTooLarge = 431,
  UnavailableForLegalReasons = 451,

  InternalServerError = 500,
  NotImplemented = 501,
  BadGateway = 502,
  ServiceUnavailable = 503,
  GatewayTimeout = 504,
  HTTPVersionNotSupported = 505,
  VariantAlsoNegotiates = 506,
  InsufficientStorage = 507,
  LoopDetected = 508,
  NotExtended = 510,
  NetworkAuthenticationRequired = 511,

  NoStatus
};

struct HttpHeader
{
  HttpHeaderEnum name;
  std::string str_repr;
  std::string value;

  HttpHeader ();
  HttpHeader (std::string _StrRepr, std::string _Value);
  HttpHeader (HttpHeaderEnum _Name, std::string _Value);

  bool
  validate ()
  {
    return int (name) != int (HttpHeaderEnum::NoHeader);
  }

  bool operator== (const HttpHeader &);
  bool operator!= (const HttpHeader &);

  ~HttpHeader () {}
};

struct HttpRequestType
{
  HttpRequestTypeEnum type;
  std::string str_repr; // String representation like "GET", "POST"
  std::string url;      // Request URL or path
  std::string version;  // HTTP version (e.g., "HTTP/1.1")

  HttpRequestType ();
  HttpRequestType (std::string _StrRepr, std::string _Url,
                   std::string _Version = "HTTP/1.1");
  HttpRequestType (HttpRequestTypeEnum _Type, std::string _Url,
                   std::string _Version = "HTTP/1.1");

  bool
  validate ()
  {
    return int (type) != int (HttpHeaderEnum::NoHeader);
  }

  bool operator== (HttpRequestType &);
  bool operator!= (HttpRequestType &);

  ~HttpRequestType () {}
};

struct HttpRequest
{
  HttpRequestType request_type;
  std::map<HttpHeaderEnum, HttpHeader> head_map;
  std::string body;

  HttpRequest ();
  HttpRequest (HttpRequestType _RequestType, std::string _Body = "");
  HttpRequest (HttpRequestType _RequestType,
               std::map<HttpHeaderEnum, HttpHeader> _Headers,
               std::string _Body = "");

  bool
  validate ()
  {
    return request_type.validate ();
  }

  inline HttpHeader
  get_header (HttpHeaderEnum he)
  {
    if (head_map.find (he) == head_map.end ())
      return HttpHeader ();

    return head_map[he];
  }

  inline HttpHeader &
  get_header_ref (HttpHeaderEnum he)
  {
    if (head_map.find (he) == head_map.end ())
      throw std::out_of_range ("Header not found");

    return head_map[he];
  }

  bool operator== (HttpRequest &);
  bool operator!= (HttpRequest &);

  ~HttpRequest () {}
};

struct HttpResponse
{
  HttpStatusEnum status_code;
  std::string status_message;
  std::string version;
  std::map<HttpHeaderEnum, HttpHeader> head_map;
  std::string body;

  HttpResponse ();
  HttpResponse (HttpStatusEnum _StatusCode, std::string _Version = "HTTP/1.1",
                std::string _Body = "");
  HttpResponse (HttpStatusEnum _StatusCode, std::string _StatusMessage,
                std::string _Version = "HTTP/1.1", std::string _Body = "");
  HttpResponse (HttpStatusEnum _StatusCode,
                std::map<HttpHeaderEnum, HttpHeader> _Headers,
                std::string _Version = "HTTP/1.1", std::string _Body = "");
  HttpResponse (HttpStatusEnum _StatusCode, std::string _StatusMessage,
                std::map<HttpHeaderEnum, HttpHeader> _Headers,
                std::string _Version = "HTTP/1.1", std::string _Body = "");

  bool
  validate ()
  {
    return status_code != HttpStatusEnum::NoStatus;
  }

  inline HttpHeader
  get_header (HttpHeaderEnum he)
  {
    if (head_map.find (he) == head_map.end ())
      return HttpHeader ();
    return head_map[he];
  }

  inline HttpHeader &
  get_header_ref (HttpHeaderEnum he)
  {
    if (head_map.find (he) == head_map.end ())
      throw std::out_of_range ("Header not found");
    return head_map[he];
  }

  bool operator== (const HttpResponse &);
  bool operator!= (const HttpResponse &);

  std::string to_string ();
  void add_body (std::string);

  ~HttpResponse () {}
};

struct RouteInfo
{
  std::vector<HttpRequestTypeEnum> allowed_requests;
  std::string path;
  std::function<HttpResponse (HttpRequest)> callback;
};

std::string get_status_message (HttpStatusEnum status_code);
HttpResponse parse_response (std::vector<std::string> &);

HttpHeader parse_header (std::string);
HttpRequestTypeEnum parse_request_type_enum (std::string);
HttpRequestType parse_request_type (std::string);
HttpRequest parse_request (std::vector<std::string> &);
} // namespace rs::block

#endif // HTTPHEADER_H
