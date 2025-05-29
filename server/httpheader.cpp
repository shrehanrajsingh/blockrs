#include "httpheader.hpp"

namespace rs::block
{
// using namespace rs::util;
static std::vector<std::pair<HttpHeaderEnum, std::string>> ntable
    = { { HttpHeaderEnum::Accept, "Accept" },
        { HttpHeaderEnum::AcceptCharset, "Accept-Charset" },
        { HttpHeaderEnum::AcceptEncoding, "Accept-Encoding" },
        { HttpHeaderEnum::AcceptLanguage, "Accept-Language" },
        { HttpHeaderEnum::AcceptRanges, "Accept-Ranges" },
        { HttpHeaderEnum::Age, "Age" },
        { HttpHeaderEnum::Allow, "Allow" },
        { HttpHeaderEnum::Authorization, "Authorization" },
        { HttpHeaderEnum::CacheControl, "Cache-Control" },
        { HttpHeaderEnum::Connection, "Connection" },
        { HttpHeaderEnum::ContentEncoding, "Content-Encoding" },
        { HttpHeaderEnum::ContentLanguage, "Content-Language" },
        { HttpHeaderEnum::ContentLength, "Content-Length" },
        { HttpHeaderEnum::ContentLocation, "Content-Location" },
        { HttpHeaderEnum::ContentMD5, "Content-MD5" },
        { HttpHeaderEnum::ContentRange, "Content-Range" },
        { HttpHeaderEnum::ContentType, "Content-Type" },
        { HttpHeaderEnum::Cookie, "Cookie" },
        { HttpHeaderEnum::Date, "Date" },
        { HttpHeaderEnum::ETag, "ETag" },
        { HttpHeaderEnum::Expect, "Expect" },
        { HttpHeaderEnum::Expires, "Expires" },
        { HttpHeaderEnum::From, "From" },
        { HttpHeaderEnum::Host, "Host" },
        { HttpHeaderEnum::IfMatch, "If-Match" },
        { HttpHeaderEnum::IfModifiedSince, "If-Modified-Since" },
        { HttpHeaderEnum::IfNoneMatch, "If-None-Match" },
        { HttpHeaderEnum::IfRange, "If-Range" },
        { HttpHeaderEnum::IfUnmodifiedSince, "If-Unmodified-Since" },
        { HttpHeaderEnum::LastModified, "Last-Modified" },
        { HttpHeaderEnum::Location, "Location" },
        { HttpHeaderEnum::MaxForwards, "Max-Forwards" },
        { HttpHeaderEnum::Pragma, "Pragma" },
        { HttpHeaderEnum::ProxyAuthenticate, "Proxy-Authenticate" },
        { HttpHeaderEnum::ProxyAuthorization, "Proxy-Authorization" },
        { HttpHeaderEnum::Range, "Range" },
        { HttpHeaderEnum::Referer, "Referer" },
        { HttpHeaderEnum::RetryAfter, "Retry-After" },
        { HttpHeaderEnum::Server, "Server" },
        { HttpHeaderEnum::SetCookie, "Set-Cookie" },
        { HttpHeaderEnum::TE, "TE" },
        { HttpHeaderEnum::Trailer, "Trailer" },
        { HttpHeaderEnum::TransferEncoding, "Transfer-Encoding" },
        { HttpHeaderEnum::Upgrade, "Upgrade" },
        { HttpHeaderEnum::UserAgent, "User-Agent" },
        { HttpHeaderEnum::Vary, "Vary" },
        { HttpHeaderEnum::Via, "Via" },
        { HttpHeaderEnum::Warning, "Warning" },
        { HttpHeaderEnum::WWWAuthenticate, "WWW-Authenticate" },
        { HttpHeaderEnum::Custom, "Custom" },
        { HttpHeaderEnum::NoHeader, "" } };

static std::vector<std::pair<HttpRequestTypeEnum, std::string>> ttable
    = { { HttpRequestTypeEnum::Get, "GET" },
        { HttpRequestTypeEnum::Post, "POST" },
        { HttpRequestTypeEnum::Put, "PUT" },
        { HttpRequestTypeEnum::Delete, "DELETE" },
        { HttpRequestTypeEnum::Head, "HEAD" },
        { HttpRequestTypeEnum::Options, "OPTIONS" },
        { HttpRequestTypeEnum::Patch, "PATCH" },
        { HttpRequestTypeEnum::Connect, "CONNECT" },
        { HttpRequestTypeEnum::Trace, "TRACE" },
        { HttpRequestTypeEnum::NoRequest, "" } };

HttpHeader::HttpHeader ()
    : name (HttpHeaderEnum::NoHeader), str_repr (""), value ("")
{
}

HttpHeader::HttpHeader (std::string _StrRepr, std::string _Value)
{
  /* check if it is a valid header */
  bool is_valid = false;
  for (auto &&i : ntable)
    {
      if (i.second == _StrRepr)
        {
          name = i.first;
          str_repr = _StrRepr;
          value = _Value;
          is_valid = true;
          break;
        }
    }

  if (!is_valid)
    {
      name = HttpHeaderEnum::Custom;
      str_repr = _StrRepr;
      value = _Value;
    }
}

HttpHeader::HttpHeader (HttpHeaderEnum _Name, std::string _Value)
{
  bool is_valid = false;
  for (auto &&i : ntable)
    {
      if (i.first == _Name)
        {
          name = _Name;
          str_repr = i.second;
          value = _Value;
          is_valid = true;
          break;
        }
    }

  if (!is_valid)
    {
      name = HttpHeaderEnum::NoHeader;
      str_repr = "";
      value = "";
    }
}

bool
HttpHeader::operator== (const HttpHeader &rhs)
{
  return name == rhs.name;
}

bool
HttpHeader::operator!= (const HttpHeader &rhs)
{
  return name != rhs.name;
}

HttpHeader
parse_header (std::string h)
{
  int colon_idx = h.find (':');
  if (colon_idx == -1)
    {
      return HttpHeader ();
    }

  std::string h_name = h.substr (0, colon_idx);
  std::string h_value = h.substr (colon_idx + 1, h.size ());

  if (h_value.find (' ') == 0)
    {
      h_value = h_value.substr (1, h_value.size ());
    }

  return HttpHeader (h_name, h_value);
}

HttpRequestType::HttpRequestType ()
    : type (HttpRequestTypeEnum::NoRequest), str_repr (""), url (""),
      version ("")
{
}

HttpRequestType::HttpRequestType (std::string _StrRepr, std::string _Url,
                                  std::string _Version)
    : url (_Url), version (_Version)
{
  /* check if it is a valid request type */
  bool is_valid = false;
  for (auto &&i : ttable)
    {
      if (i.second == _StrRepr)
        {
          type = i.first;
          str_repr = _StrRepr;
          is_valid = true;
          break;
        }
    }

  if (!is_valid)
    {
      type = HttpRequestTypeEnum::NoRequest;
      str_repr = "";
    }
}

HttpRequestType::HttpRequestType (HttpRequestTypeEnum _Type, std::string _Url,
                                  std::string _Version)
    : type (_Type), url (_Url), version (_Version)
{
  bool is_valid = false;
  for (auto &&i : ttable)
    {
      if (i.first == _Type)
        {
          str_repr = i.second;
          is_valid = true;
          break;
        }
    }

  if (!is_valid)
    {
      type = HttpRequestTypeEnum::NoRequest;
      str_repr = "";
    }
}

bool
HttpRequestType::operator== (const HttpRequestType &rhs)
{
  return type == rhs.type;
}

bool
HttpRequestType::operator!= (const HttpRequestType &rhs)
{
  return type != rhs.type;
}

HttpRequestTypeEnum
parse_request_type_enum (std::string s)
{
  int space_pos = s.find (' ');
  std::string method = (space_pos != -1) ? s.substr (0, space_pos) : s;

  for (const auto &entry : ttable)
    {
      if (entry.second == method)
        {
          return entry.first;
        }
    }
  return HttpRequestTypeEnum::NoRequest;
}

HttpRequestType
parse_request_type (std::string s)
{
  int first_space = s.find (' ');
  if (first_space == -1)
    {
      return HttpRequestType ();
    }

  int second_space = s.find (' ', first_space + 1);
  if (second_space == -1)
    {
      return HttpRequestType ();
    }

  HttpRequestTypeEnum rte = parse_request_type_enum (s);
  std::string url = s.substr (first_space + 1, second_space - first_space - 1);
  std::string version = s.substr (second_space + 1, s.size ());

  return HttpRequestType (rte, url, version);
}

HttpRequest::HttpRequest () : request_type (HttpRequestType ()), body ("") {}

HttpRequest::HttpRequest (HttpRequestType _RequestType, std::string _Body)
    : request_type (_RequestType), body (_Body)
{
}

HttpRequest::HttpRequest (HttpRequestType _RequestType,
                          std::vector<HttpHeader> _Headers, std::string _Body)
    : request_type (_RequestType), headers (_Headers), body (_Body)
{
  map_headers ();
}

void
HttpRequest::map_headers ()
{
  for (HttpHeader &i : headers)
    head_map[i.name] = i;
}

bool
HttpRequest::operator== (const HttpRequest &rhs)
{
  if (headers.size () != rhs.headers.size ())
    return false;

  bool request_match = request_type == rhs.request_type;
  bool body_match = body == rhs.body;

  if (!request_match || !body_match)
    return false;

  for (std::size_t i = 0; i < headers.size (); ++i)
    {
      bool found_match = false;
      for (std::size_t j = 0; j < rhs.headers.size (); ++j)
        {
          if (headers[i] == rhs.headers[j]
              && headers[i].value == rhs.headers[j].value)
            {
              found_match = true;
              break;
            }
        }
      if (!found_match)
        return false;
    }

  return true;
}

bool
HttpRequest::operator!= (const HttpRequest &rhs)
{
  return !(*this == rhs);
}

HttpRequest
parse_request (std::vector<std::string> &rhs)
{
  if (rhs.size () == 0)
    {
      return HttpRequest ();
    }

  std::string request_line = rhs[0];
  int first_space = request_line.find (' ');
  if (first_space == -1)
    {
      return HttpRequest ();
    }

  int second_space = request_line.find (' ', first_space + 1);
  if (second_space == -1)
    {
      return HttpRequest ();
    }

  HttpRequestTypeEnum rte = parse_request_type_enum (request_line);
  std::string url
      = request_line.substr (first_space + 1, second_space - first_space - 1);
  std::string version
      = request_line.substr (second_space + 1, request_line.size ());

  HttpRequestType req_type (rte, url, version);

  std::vector<HttpHeader> headers;
  size_t i = 1;
  for (; i < rhs.size (); ++i)
    {
      if (rhs[i].size () == 1 || rhs[i].empty ())
        {
          ++i;
          break;
        }

      int colon_idx = rhs[i].find (':');
      if (colon_idx != -1)
        {
          std::string h_name = rhs[i].substr (0, colon_idx);
          std::string h_value = rhs[i].substr (colon_idx + 1, rhs[i].size ());

          if (h_value.find (' ') == 0)
            {
              h_value = h_value.substr (1, h_value.size ());
            }

          //   std::cout << "Header: " << h_name << " = " << h_value <<
          //   std::endl;
          headers.push_back (HttpHeader (h_name, h_value));
          //   std::cout << "Header: " << headers.back ().str_repr << " = "
          //             << headers.back ().value << std::endl;
        }
    }

  //   for (size_t j = 0; j < headers.size(); ++j)
  //     {
  //       std::cout << "Header: " << headers[j].str_repr << " = "
  //                 << headers[j].value << std::endl;
  //     }

  std::string body;
  for (; i < rhs.size (); ++i)
    {
      if (!body.empty ())
        body += "\n";

      body += rhs[i];
    }

  return HttpRequest (req_type, headers, body);
}
} // namespace rs::block
