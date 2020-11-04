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


#ifndef HTTP_HEADER_H_
#define HTTP_HEADER_H_

#include "ns3/header.h"
#include <map>

//using namespace std;

namespace ns3 {

/**
 * \class HttpHeader.
 * \brief Packet header for HTTP.
 */
class HttpHeader :public Header
{
public:
  /**
   * \brief Construct a null HTTP header.
   */
  HttpHeader();

  /**
   * \brief Set the message as Request or Response.
   * \param request boolean value.
   */
  void SetRequest (bool request);

  /**
   * \brief Get the message type (Request or Response).
   * \return the type of the message True=Request, False=Response.
   */
  bool GetRequest (void) const;

  /**
   * \brief Set the method field of the request message.
   * \param method the string with the method GET, HEAD, POST.
   */
  void SetMethod (std::string method);

  /**
   * \brief Get the method field.
   * \return the method field of the request message.
   */
  std::string GetMethod (void) const;

  /**
   * \brief Set the url field of the request message.
   * \param url the string with the url of the object.
   */
  void SetUrl (std::string url);

  /**
   * \brief Get the url field.
   * \return the url field of the request message.
   */
  std::string GetUrl (void) const;

  /**
   * \brief Set the version field.
   * \param version the string with the version of the HTTP protocol (HTTP/1.0 or HTTP/1.1).
   */
  void SetVersion (std::string version);

  /**
   * \brief Get the version field.
   * \return the version field.
   */
  std::string GetVersion (void) const;

  /**
   * \brief Set the Status Code field of the response message.
   * \param statusCode the string with the status code (200, 301, 404) of the response message.
   */
  void SetStatusCode (std::string statusCode);

  /**
   * \brief Get the status code field.
   * \return the status code field.
   */
  std::string GetStatusCode (void) const;

  /**
   * \brief Set the Phrase field of the response message.
   * \param phrase the string with the phrase (OK, NOT FOUND) of the response message.
   */
  void SetPhrase (std::string phrase);

  /**
   * \brief Get the phrase field.
   * \return the phrase field.
   */
  std::string GetPhrase (void) const;

  /**
   * \brief Set the Header Field of the HTTP Message
   * \param headerFieldName the name of the header field.
   * \param headerFieldValue the value of the header field.
   */
  void SetHeaderField(std::string headerFieldName, std::string headerFieldValue);

  /**
   * \brief Set the Header Field of the HTTP Message
   * \param headerFieldName the name of the header field.
   * \param headerFieldValue the value of the header field.
   */
  void SetHeaderField(std::string headerFieldName, uint32_t headerFieldValue);

  /**
   * \brief Get the Header Field of the HTTP Message
   * \param headerFieldName the name of the header field.
   * \return the header field
   */
  std::string GetHeaderField(std::string headerFieldName);

  /**
   * \brief Print some informations about the header.
   * \param os output stream.
   * \return info about this header.
   */
  virtual void Print (std::ostream &os) const;

  /**
   * \brief Serialize the header.
   * \param start Buffer iterator.
   */
  virtual void Serialize (Buffer::Iterator start) const;

  /**
   * \brief Deserialize the header.
   * \param start Buffer iterator.
   * \return size of the header.
   */
  virtual uint32_t Deserialize (Buffer::Iterator start);

  /**
   * \brief Get the serialized size of the header.
   * \return size.
   */
  virtual uint32_t GetSerializedSize (void) const;

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

private:

  bool   m_request;

  /**
   * \brief The method field.
   */
  std::string m_method;

  /**
   * \brief The url field.
   */
  std::string m_url;

  /**
   * \brief The version field.
   */
  std::string m_version;

  /**
   * \brief The Status Code field.
   */
  std::string m_statusCode;

  /**
   * \brief The Phrase field.
   */
  std::string m_phrase;

  /**
   * \brief Map for Header Fields.
   */
  std::map<std::string, std::string> m_headerFieldMap;

  /**
   * \brief Iterator for Map Header Fields.
   */
  std::map<std::string, std::string>::iterator m_headerFieldMapIt;
};

}

#endif /* HTTP_HEADER_H_ */
