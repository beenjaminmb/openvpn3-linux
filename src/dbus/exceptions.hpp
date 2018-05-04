//  OpenVPN 3 Linux client -- Next generation OpenVPN client
//
//  Copyright (C) 2017      OpenVPN Inc. <sales@openvpn.net>
//  Copyright (C) 2017      David Sommerseth <davids@openvpn.net>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Affero General Public License as
//  published by the Free Software Foundation, version 3 of the
//  License.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Affero General Public License for more details.
//
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#ifndef OPENVPN3_DBUS_EXCEPTIONS_HPP
#define OPENVPN3_DBUS_EXCEPTIONS_HPP

#include <exception>
#include <sstream>

#include "config.h"

namespace openvpn
{
    /**
     *   DBusException is thrown whenever a D-Bus related error happens
     */
    class DBusException : public std::exception
    {
    public:
        DBusException(const std::string classn, const std::string& err, const char *filen, const unsigned int linenum, const char *fn) noexcept
        : errorstr(err)
        {
            prepare_sourceref(classn, filen, linenum, fn, errorstr);
        }


        DBusException(const std::string classn, std::string&& err, const char *filen, const unsigned int linenum, const char *fn) noexcept
        : errorstr(std::move(err))
        {
            prepare_sourceref(classn, filen, linenum, fn, errorstr);
        }


        virtual ~DBusException() throw() {}


        virtual const char* what() const throw()
        {
#ifdef DEBUG_EXCEPTIONS
	    return sourceref.c_str();
#else
	    return errorstr.c_str();
#endif
        }


        const std::string& err() const noexcept
        {
            return std::move(std::string(sourceref + errorstr));
        }


        std::string getRawError() const noexcept
        {
            return errorstr;
        }


    protected:
        std::string sourceref;
        const std::string errorstr;

    private:
        void prepare_sourceref(const std::string classn,
                               const char *filen, const unsigned int linenum,
                               const char *fn, const std::string err) noexcept
        {
            sourceref = "{" + std::string(filen) + ":" + std::to_string(linenum)
                      + ", " + classn + "::" + std::string(fn) + "()} " + err;
        }

    };

#define THROW_DBUSEXCEPTION(classname, fault_data) throw DBusException(classname, fault_data, __FILE__, __LINE__, __FUNCTION__)

    /**
     *  Specian exception classes used by set/get properties calls.
     *  exceptions will be translated into a D-Bus error which the
     *  calling D-Bus client will receive.
     */

    class DBusPropertyException : public std::exception
    {
    public:
        DBusPropertyException(GQuark domain,
                              gint code,
                              const std::string& interf,
                              const std::string& objp,
                              const std::string& prop,
                              const std::string& err) noexcept
        : errordomain(domain),
            errorcode(code),
            errorstr(err),
            detailed("")
        {
            detailed = std::string("[interface=") + interf
                        + std::string(", path=") + objp
                        + std::string(", property=") + prop
                        + std::string("] ")
                        + errorstr;
        }


        DBusPropertyException(GQuark domain,
                              gint code,
                              const std::string&& interf,
                              const std::string&& objp,
                              const std::string&& prop,
                              const std::string&& err) noexcept
        : errordomain(domain),
            errorcode(code),
            errorstr(err),
            detailed("")
        {
            detailed = std::string("[interface=") + interf
                        + std::string(", path=") + objp
                        + std::string(", property=") + prop
                        + std::string("] ")
                        + errorstr;
        }


        virtual ~DBusPropertyException() throw() {}


        virtual const char* what() const throw()
        {
            return detailed.c_str();
        }


        const std::string& err() const noexcept
        {
            return std::move(detailed);
        }


        std::string getRawError() const noexcept
        {
            return errorstr;
        }


        void SetDBusError(GError **error)
        {
            g_set_error(error, errordomain, errorcode,
                        "%s", errorstr.c_str());
        }


    private:
        GQuark errordomain;
        guint errorcode;
        std::string errorstr;
        std::string detailed;
    };
};
#endif // OPENVPN3_DBUS_EXCEPTIONS_HPP
