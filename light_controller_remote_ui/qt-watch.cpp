/***
  This file is part of avahi.
 
  avahi is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) any later version.
 
  avahi is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
  Public License for more details.
 
  You should have received a copy of the GNU Lesser General Public
  License along with avahi; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
  USA.
***/

#include <sys/time.h>
#include <avahi-common/timeval.h>
#include "qt-watch.h"


AvahiWatch::AvahiWatch(int fd, AvahiWatchEvent event, AvahiWatchCallback callback, void* userdata) : 
    m_in(0), m_out(0),  m_callback(callback), m_fd(fd), m_userdata(userdata), m_incallback(false) {

    setWatchedEvents(event);
}

void AvahiWatch::gotIn() {
    m_lastEvent = AVAHI_WATCH_IN;
    m_incallback = true;
    m_callback(this,m_fd,m_lastEvent,m_userdata);
    m_incallback = false;
}

void AvahiWatch::gotOut() {
    m_lastEvent = AVAHI_WATCH_IN;
    m_incallback = true;
    m_callback(this,m_fd,m_lastEvent,m_userdata);
    m_incallback=false;
}

void AvahiWatch::setWatchedEvents(AvahiWatchEvent event) {
    if (!(event & AVAHI_WATCH_IN)) {
        delete m_in;
        m_in=0;
    } /* endif */
    if (!(event & AVAHI_WATCH_OUT)) {
        delete m_out;
        m_out=0;
    } /* endif */
    if (event & AVAHI_WATCH_IN) { 
        m_in = new QSocketNotifier(m_fd,QSocketNotifier::Read, this);
        connect(m_in,SIGNAL(activated(int)),SLOT(gotIn()));
    } /* endif */
    if (event & AVAHI_WATCH_OUT) { 
        m_out = new QSocketNotifier(m_fd,QSocketNotifier::Write, this);
        connect(m_out,SIGNAL(activated(int)),SLOT(gotOut()));
    } /* endif */
}    

AvahiTimeout::AvahiTimeout(const struct timeval* tv, AvahiTimeoutCallback callback, void *userdata) : 
    m_callback(callback), m_userdata(userdata)
{
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
    m_timer.setSingleShot(true);
    update(tv);
}

void AvahiTimeout::update(const struct timeval *tv) {
    m_timer.stop();
    if (tv) {
        AvahiUsec u = avahi_age(tv)/1000;
        m_timer.start( (u>0) ? 0 : -u);
    } /* endif */
}

void AvahiTimeout::timeout() {
    m_callback(this,m_userdata);
}

static AvahiWatch* q_watch_new(const AvahiPoll *, int fd,
                               AvahiWatchEvent event, AvahiWatchCallback callback,
                               void *userdata) {
    return new AvahiWatch(fd, event, callback, userdata);
}

static void q_watch_update(AvahiWatch *w, AvahiWatchEvent events) {
    w->setWatchedEvents(events);
}

static AvahiWatchEvent q_watch_get_events(AvahiWatch *w) {
    return w->getEvents();
}
    
static void q_watch_free(AvahiWatch *w) {
    delete w;
}
    
static AvahiTimeout* q_timeout_new(const AvahiPoll *, const struct timeval *tv, AvahiTimeoutCallback callback,
    void *userdata) {
    return new AvahiTimeout(tv, callback, userdata);
}

static void q_timeout_update(AvahiTimeout *t, const struct timeval *tv)  {
    t->update(tv);
}

static void q_timeout_free(AvahiTimeout *t)  {
    delete t;
}

const AvahiPoll* avahi_qt_poll_get(void)  {
    static const AvahiPoll qt_poll = {
        NULL,
        q_watch_new,
        q_watch_update,
        q_watch_get_events,
        q_watch_free,
        q_timeout_new,
        q_timeout_update,
        q_timeout_free
    };

    return &qt_poll;
}
