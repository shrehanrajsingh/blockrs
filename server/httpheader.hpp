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

  bool operator== (const HttpRequestType &);
  bool operator!= (const HttpRequestType &);

  ~HttpRequestType () {}
};

struct HttpRequest
{
  HttpRequestType request_type;
  std::vector<HttpHeader> headers;
  std::map<HttpHeaderEnum, HttpHeader> head_map;
  std::string body;

  HttpRequest ();
  HttpRequest (HttpRequestType _RequestType, std::string _Body = "");
  HttpRequest (HttpRequestType _RequestType, std::vector<HttpHeader> _Headers,
               std::string _Body = "");

  bool
  validate ()
  {
    return request_type.validate ();
  }

  void map_headers (); /* for faster header access */

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

  bool operator== (const HttpRequest &);
  bool operator!= (const HttpRequest &);

  ~HttpRequest () {}
};

HttpHeader parse_header (std::string);
HttpRequestTypeEnum parse_request_type_enum (std::string);
HttpRequestType parse_request_type (std::string);
HttpRequest parse_request (std::vector<std::string> &);
} // namespace rs::block

#endif // HTTPHEADER_H
