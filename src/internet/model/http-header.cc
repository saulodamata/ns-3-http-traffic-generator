/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2020 Saulo da Mata
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Saulo da Mata <damata.saulo@gmail.com>
 */

#include "ns3/log.h"
#include "http-header.h"


NS_LOG_COMPONENT_DEFINE ("HttpHeader");

//using namespace std;

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (HttpHeader);

HttpHeader::HttpHeader ()
  : m_request(true),
    m_method(""),
    m_url(""),
    m_version(""),
    m_statusCode(""),
    m_phrase("")
{
  NS_LOG_FUNCTION_NOARGS ();
}


void
HttpHeader::SetRequest (bool request)
{
  NS_LOG_FUNCTION (this << request);
  m_request = request;
}

bool
HttpHeader::GetRequest (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_request;
}

void
HttpHeader::SetMethod (std::string method)
{
  NS_LOG_FUNCTION (this << method);
  m_method = method;
}

std::string
HttpHeader::GetMethod (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_method;
}

void
HttpHeader::SetUrl (std::string url)
{
  NS_LOG_FUNCTION (this << url);
  m_url = url;
}

std::string
HttpHeader::GetUrl (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_url;
}

void
HttpHeader::SetVersion (std::string version)
{
  NS_LOG_FUNCTION (this << version);
  m_version = version;
}

std::string
HttpHeader::GetVersion (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_version;
}

void
HttpHeader::SetStatusCode (std::string statusCode)
{
  NS_LOG_FUNCTION (this << statusCode);
  m_statusCode = statusCode;
}

std::string
HttpHeader::GetStatusCode (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_statusCode;
}

void
HttpHeader::SetPhrase (std::string phrase)
{
  NS_LOG_FUNCTION (this << phrase);
  m_phrase = phrase;
}

std::string
HttpHeader::GetPhrase (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_phrase;
}

void
HttpHeader::SetHeaderField (std::string headerFieldName, std::string headerFieldValue)
{
    NS_LOG_FUNCTION (this << headerFieldName << ": " << headerFieldValue);
    m_headerFieldMap.insert (std::pair<std::string, std::string>(headerFieldName, headerFieldValue));
}

void
HttpHeader::SetHeaderField (std::string headerFieldName, uint32_t headerFieldValue)
{
    NS_LOG_FUNCTION (this << headerFieldName << ": " << headerFieldValue);
    std::stringstream ss;
    ss  << headerFieldValue;
    m_headerFieldMapIt = m_headerFieldMap.find(headerFieldName);
    if(m_headerFieldMapIt == m_headerFieldMap.end())
      {
        m_headerFieldMap.insert (std::pair<std::string, std::string>(headerFieldName, ss.str()));
      }
    else
      {
        m_headerFieldMapIt->second = ss.str();
      }
}

std::string
HttpHeader::GetHeaderField (std::string headerFieldName)
{
    NS_LOG_FUNCTION (this << headerFieldName);

    m_headerFieldMapIt = m_headerFieldMap.find(headerFieldName);
    if(m_headerFieldMapIt != m_headerFieldMap.end())
      {
          return m_headerFieldMapIt->second;
      }
    else
      {
          NS_LOG_ERROR("Header Field: " << headerFieldName <<
                                 " does not exist. It has not been set by the remote side.");
          return "";
      }
}

TypeId
HttpHeader::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  static TypeId tid = TypeId ("ns3::HttpHeader")
    .SetParent<Header> ()
    .AddConstructor<HttpHeader> ()
  ;
  return tid;
}
TypeId
HttpHeader::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return GetTypeId ();
}

void
HttpHeader::Print (std::ostream &os) const
{
  NS_LOG_FUNCTION_NOARGS ();

  if (m_request)
    {
      os << "\nmethod:" << m_method << " "
         << "URL:" << m_url << " "
         << "Version:" << m_version << "\n"
       ;
    }
  else
    {
      os << "\nVersion:" << m_version << " "
               << "Status Code:" << m_statusCode << " "
               << "Phrase:" << m_phrase << "\n"
               ;

      std::map<std::string, std::string>::const_iterator it = m_headerFieldMap.begin();
      for(it = m_headerFieldMap.begin(); it != m_headerFieldMap.end(); it++)
        {
          os << it->first.length() << ": " << it->second.length() << "\n";
        }
    }
}

uint32_t
HttpHeader::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  if(m_request)
    {
      //                                                                                         spaces + CR/LF + NULL
      return m_method.length() + m_url.length() + m_version.length() +  2   +     4   +     1;
    }
  else
    {
      //                                                                                                             spaces + CR/LF
      uint32_t size = m_version.length() + m_statusCode.length() + m_phrase.length() + 2    +     2;
      std::map<std::string, std::string>::const_iterator it = m_headerFieldMap.begin();
      for(it = m_headerFieldMap.begin(); it != m_headerFieldMap.end(); it++)
        {
          //                                space/dots                              CR/LF
          size += it->first.length()  +    2     + it->second.length() +  2;
        }

      //Counting last CR/LF + NULL
      size += 2 + 1;

      return size;
    }
}


void
HttpHeader::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION_NOARGS ();

  if(m_request)
    {
      std::string requestLine = m_method + " " + m_url + " " + m_version + "\r\n\r\n";
      char tmpBuffer [requestLine.length() + 1];
      strcpy (tmpBuffer, requestLine.c_str());
      start.Write ((uint8_t *)tmpBuffer, strlen(tmpBuffer) + 1);
    }
  else
    {
      std::string statusLine = m_version + " " + m_statusCode + " " + m_phrase + "\r\n";

      std::map<std::string, std::string>::const_iterator it = m_headerFieldMap.begin();
      for(it = m_headerFieldMap.begin(); it != m_headerFieldMap.end(); it++)
        {
          statusLine += it->first + ": " + it->second + "\r\n";
        }

      statusLine += "\r\n";

      char tmpBuffer [statusLine.length() + 1];
      strcpy (tmpBuffer, statusLine.c_str());
      start.Write ((uint8_t *)tmpBuffer, strlen(tmpBuffer) + 1);
    }
}


uint32_t
HttpHeader::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION_NOARGS ();
  Buffer::Iterator i = start;
  uint8_t c;
  uint32_t len = 0;

  do
    {
      c  = i.ReadU8 ();
      len++;
    } while (c != 0);


  char tmpBuffer [len];
  start.Read ((uint8_t*)tmpBuffer, len);


  std::string tmpString = tmpBuffer;
  uint16_t begin = 0;
  uint16_t end = tmpString.find_first_of (" ");
  std::string firstField = tmpString.substr (begin, end - begin);

  if(firstField == "GET")
    m_request = true;
  else
    m_request = false;

  if(m_request)
    {
      m_method = firstField;
      begin = end + 1;
      end = tmpString.find_first_of (" ", begin);

      m_url = tmpString.substr (begin, end - begin);
      begin = end + 1;
      end = tmpString.find_first_of (" ", begin);

      m_version = tmpString.substr (begin, end - begin);
    }
  else
    {
      m_version = firstField;

      begin = end + 1;
      end = tmpString.find_first_of (" ", begin);
      m_statusCode = tmpString.substr (begin, end - begin);

      begin = end + 1;
      end = tmpString.find_first_of ("\r", begin);
      m_phrase = tmpString.substr (begin, end - begin);


      size_t cr = tmpString.find_first_of("\r");
      while (cr != std::string::npos)
        {
          begin = cr + 2;
          end = tmpString.find_first_of("\r", begin);
          if (begin != end)
            {
              std::string headerField = tmpString.substr(begin, end - begin);

              end=  headerField.find_first_of(":");
              begin = end + 2;
              std::string headerFieldName = headerField.substr(0, end);
              end = headerField.find_first_of(":", begin);
              std::string headerFieldValue = headerField.substr(begin, end - begin);

              m_headerFieldMapIt = m_headerFieldMap.find(headerFieldName);
              if(m_headerFieldMapIt == m_headerFieldMap.end())
                {
                  m_headerFieldMap.insert (std::pair<std::string, std::string>(headerFieldName, headerFieldValue));
                }
              else
                {
                  m_headerFieldMapIt->second = headerFieldValue;
                }

              cr = tmpString.find_first_of("\r", tmpString.find_first_of("\r", cr + 2));
            }
          else
            {
              cr = std::string::npos;
            }
         }
    }

  return GetSerializedSize ();
}


}

