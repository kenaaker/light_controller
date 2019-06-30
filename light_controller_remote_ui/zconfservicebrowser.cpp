/*
 *  This file is part of qtzeroconf. (c) 2012 Johannes Hilden
 *  https://github.com/johanneshilden/qtzeroconf
 *
 *  qtzeroconf is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation; either version 2.1 of the
 *  License, or (at your option) any later version.
 *
 *  qtzeroconf is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
 *  Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with qtzeroconf; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#include <QHash>
#include <QStringBuilder>
#include <QDebug>
#include <cassert>
#include <avahi-common/error.h>
#include "zconfservicebrowser.h"
#include "zconfserviceclient.h"

/*!
    \struct ZConfServiceEntry

    \brief This struct contains information about a particular Zeroconf service
    available on the local network.
 */

/*!
    \property QString ZConfServiceEntry::ip

    A string representation of the IPv4 or IPv6 IP address associated with this
    service.
 */

/*!
    \property QString ZConfServiceEntry::domain

    The domain associated with this service.
 */

/*!
    \property QString ZConfServiceEntry::host

    The host name associated with this service.
 */

/*!
    \property uint16_t ZConfServiceEntry::port

    The IP port number associated with this service.
 */

/*!
    A human-readable string representation of the network layer protocol used
    by this service. Possible values are "IPv4", "IPv6", and "Unspecified".
 */
QString ZConfServiceEntry::protocolName() const
{
    switch (protocol)
    {
    case AVAHI_PROTO_INET:  return "IPv4";
    case AVAHI_PROTO_INET6: return "IPv6";
    default:                return "Unspecified";
    }
}

/*!
    \fn bool ZConfServiceEntry::isCached() const

    Returns true if this response originates from the cache.
 */

/*!
    \fn bool ZConfServiceEntry::isWideArea() const

    Returns true if this response originates from wide area DNS.
 */

/*!
    \fn bool ZConfServiceEntry::isMulticast() const

    Returns true if this response originates from multicast DNS.
 */

/*!
    \fn bool ZConfServiceEntry::isLocal() const

    Returns true if this service resides on and was announced by the local host.
 */

class ZConfServiceBrowserPrivate
{
public:
    ZConfServiceBrowserPrivate(ZConfServiceClient *client)
        : client(client), browser(0)
    {
    }

    static void callback(AvahiServiceBrowser     *browser,
                         AvahiIfIndex             interface,
                         AvahiProtocol            protocol,
                         AvahiBrowserEvent        event,
                         const char              *name,
                         const char              *type,
                         const char              *domain,
                         AvahiLookupResultFlags   flags,
                         void                    *userdata)
    {
        Q_UNUSED(browser);
        Q_UNUSED(flags);

        ZConfServiceBrowser *serviceBrowser = static_cast<ZConfServiceBrowser *>(userdata);
        if (serviceBrowser) {
            switch (event) {
            case AVAHI_BROWSER_FAILURE:
                qDebug() << ("Avahi browser error: " % QString(avahi_strerror(avahi_client_errno(serviceBrowser->d_ptr->client->client))));
                break;
            case AVAHI_BROWSER_NEW:
                qDebug() << ("New service '" % QString(name) % "' of type " % QString(type) % " in domain " % QString(domain) % ".");

                // We ignore the returned resolver object. In the callback
                // function we free it. If the server is terminated before
                // the callback function is called the server will free
                // the resolver for us.
                if (!(avahi_service_resolver_new(serviceBrowser->d_ptr->client->client,
                                                 interface,
                                                 protocol,
                                                 name,
                                                 serviceBrowser->d_ptr->type.toLatin1().data(),
                                                 domain,
                                                 AVAHI_PROTO_UNSPEC,
                                                 (AvahiLookupFlags) 0,
                                                 ZConfServiceBrowserPrivate::resolve,
                                                 serviceBrowser)))
                    qDebug() << ("Failed to resolve service '" % QString(name) % "': " % avahi_strerror(avahi_client_errno(serviceBrowser->d_ptr->client->client)));
                break;
            case AVAHI_BROWSER_REMOVE:
                serviceBrowser->d_ptr->entries.remove(name);
                emit serviceBrowser->serviceEntryRemoved(name);
                qDebug() << "Service '" % QString(name) % "' removed from the network.";
                break;
            case AVAHI_BROWSER_ALL_FOR_NOW:
            case AVAHI_BROWSER_CACHE_EXHAUSTED:
                qDebug() << (AVAHI_BROWSER_ALL_FOR_NOW == event
                             ? "AVAHI_BROWSER_ALL_FOR_NOW"
                             : "AVAHI_BROWSER_CACHE_EXHAUSTED");
            } // end switch
        }
    }

    static void resolve(AvahiServiceResolver   *resolver,
                        AvahiIfIndex            interface,
                        AvahiProtocol           protocol,
                        AvahiResolverEvent      event,
                        const char             *name,
                        const char             *type,
                        const char             *domain,
                        const char             *host_name,
                        const AvahiAddress     *address,
                        uint16_t                port,
                        AvahiStringList        *txt,
                        AvahiLookupResultFlags  flags,
                        void                   *userdata)
{
        Q_UNUSED(interface);
        Q_UNUSED(type);
        Q_UNUSED(txt);

        ZConfServiceBrowser *serviceBrowser = static_cast<ZConfServiceBrowser *>(userdata);
        if (serviceBrowser) {
            switch (event) {
                case AVAHI_RESOLVER_FAILURE:
                    qDebug() << ("Failed to resolve service '" % QString(name) % "': " % avahi_strerror(avahi_client_errno(serviceBrowser->d_ptr->client->client)));
                    break;
                case AVAHI_RESOLVER_FOUND:
                {
                    char a[AVAHI_ADDRESS_STR_MAX];
                    avahi_address_snprint(a, sizeof(a), address);
                    ZConfServiceEntry entry;
                    entry.ip        = QString(a);
                    entry.domain    = domain;
                    entry.host      = host_name;
                    entry.port      = port;
                    entry.protocol  = protocol;
                    entry.flags     = flags;
                    serviceBrowser->d_ptr->entries.insert(name, entry);
                    emit serviceBrowser->serviceEntryAdded(name);
                }
            }
            avahi_service_resolver_free(resolver);
        }
    }

    typedef QHash<QString, ZConfServiceEntry> ZConfServiceEntryTable;

    ZConfServiceClient      *const client;
    AvahiServiceBrowser     *browser;
    ZConfServiceEntryTable   entries;
    QString                  type;
};

/*!
    \class ZConfServiceBrowser

    \brief AvahiServiceBrowser wrapper that lets you browse for services
    available on the local network. This class can be used to handle Zeroconf
    service discovery in a Qt-based client application.

    Instantiate a ZConfServiceBrowser object and call browse() with the desired
    service type as argument (e.g., "_http._tcp" or "_ipp._tcp").

    ZConfServiceBrowser will emit serviceEntryAdded() when a new service is
    discovered and serviceEntryRemoved() when a service is removed from the
    network.
 */

/*!
    Creates a Zeroconf service browser. Call browse() to start browsing for
    services.
 */
ZConfServiceBrowser::ZConfServiceBrowser(QObject *parent)
    : QObject(parent),
      d_ptr(new ZConfServiceBrowserPrivate(new ZConfServiceClient(this)))
{
    connect(d_ptr->client, SIGNAL(clientRunning()), this, SLOT(createServiceBrowser()));
}

/*!
    Destroys the browser object and releases all resources associated with it.
 */
ZConfServiceBrowser::~ZConfServiceBrowser()
{
    if (d_ptr->browser)
        avahi_service_browser_free(d_ptr->browser);
    delete d_ptr;
}

/*!
    Browses for Zeroconf services on the LAN. This is a non-blocking call.
    ZConfServiceBrowser will emit serviceEntryAdded() when a new service is
    discovered and serviceEntryRemoved() when a service is removed from the
    network.
 */
void ZConfServiceBrowser::browse(QString serviceType)
{
    d_ptr->type = serviceType;
    assert(d_ptr->client);
    d_ptr->client->run();
}

/*!
    Returns a ZConfServiceEntry struct with detailed information about the
    Zeroconf service associated with the name.
 */
ZConfServiceEntry ZConfServiceBrowser::serviceEntry(QString name)
{
    return d_ptr->entries.value(name);
}

void ZConfServiceBrowser::createServiceBrowser()
{
    if (d_ptr->browser)
        return;
    d_ptr->browser = avahi_service_browser_new(d_ptr->client->client,
                                               AVAHI_IF_UNSPEC,
                                               AVAHI_PROTO_UNSPEC,
                                               d_ptr->type.toLatin1().data(),
                                               NULL,
                                               (AvahiLookupFlags) 0,
                                               ZConfServiceBrowserPrivate::callback,
                                               this);
}
