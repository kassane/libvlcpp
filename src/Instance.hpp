/*****************************************************************************
 * libvlc_Instance.hpp: Instance API
 *****************************************************************************
 * Copyright © 2014 the VideoLAN team
 *
 * Authors: Alexey Sokolov <alexey@alexeysokolov.co.cc>
 *          Hugo Beauzée-Luyssen <hugo@beauzee.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifndef LIBVLC_CXX_INSTANCE_H
#define LIBVLC_CXX_INSTANCE_H

#include <string>
#include <vector>
#include "common.hpp"
#include "Internal.hpp"
#include "structures.hpp"

namespace VLC
{

class VLCPP_API Instance : public Internal<libvlc_instance_t>
{
public:
    Instance();
    /**
     * Copy libvlc_instance_t from another to new Instance object.
     * \param another existing Instance
     */
    Instance(const Instance& another);

    /**
     * Copy libvlc_instance_t from another Instance
     * to this Instance
     * \param another existing Instance
     */
    Instance& operator=(const Instance& another);

    /**
     * Check if 2 Instance objects contain the same libvlc_instance_t.
     * \param another another Instance
     * \return true if they contain the same libvlc_instance_t
     */
    bool operator==(const Instance& another) const;

    ~Instance();

    /**
     * Create and initialize a libvlc instance. This functions accept a list
     * of "command line" arguments similar to the main(). These arguments
     * affect the LibVLC instance default configuration.
     *
     * \version Arguments are meant to be passed from the command line to
     * LibVLC, just like VLC media player does. The list of valid arguments
     * depends on the LibVLC version, the operating system and platform, and
     * set of available LibVLC plugins. Invalid or unsupported arguments will
     * cause the function to fail (i.e. return NULL). Also, some arguments
     * may alter the behaviour or otherwise interfere with other LibVLC
     * functions.
     *
     * \warning There is absolutely no warranty or promise of forward,
     * backward and cross-platform compatibility with regards to
     * Instance::Instance() arguments. We recommend that you do not use them,
     * other than when debugging.
     *
     * \param argc  the number of arguments (should be 0)
     *
     * \param argv  list of arguments (should be NULL)
     */
    static Instance create(int argc, const char *const * argv);

    /**
     * Try to start a user interface for the libvlc instance.
     *
     * \param name  interface name, or NULL for default
     *
     * \return 0 on success, -1 on error.
     */
    int addIntf(const std::string& name);

    /**
     * Registers a callback for the LibVLC exit event. This is mostly useful
     * if the VLC playlist and/or at least one interface are started with
     * libvlc_playlist_play() or Instance::addIntf() respectively. Typically,
     * this function will wake up your application main loop (from another
     * thread).
     *
     * \note This function should be called before the playlist or interface
     * are started. Otherwise, there is a small race condition: the exit
     * event could be raised before the handler is registered.
     *
     * \param cb  callback to invoke when LibVLC wants to exit, or NULL to
     * disable the exit handler (as by default)
     *
     * \param opaque  data pointer for the callback
     *
     * \warning This function and Instance::wait() cannot be used at the same
     * time.
     */
    void setExitHandler(void(*cb)(void *), void * opaque);

    /**
     * Sets the application name. LibVLC passes this as the user agent string
     * when a protocol requires it.
     *
     * \param name  human-readable application name, e.g. "FooBar player
     * 1.2.3"
     *
     * \param http  HTTP User Agent, e.g. "FooBar/1.2.3 Python/2.6.0"
     *
     * \version LibVLC 1.1.1 or later
     */
    void setUserAgent(const std::string& name, const std::string& http);

    /**
     * Sets some meta-information about the application. See also
     * Instance::setUserAgent() .
     *
     * \param id  Java-style application identifier, e.g. "com.acme.foobar"
     *
     * \param version  application version numbers, e.g. "1.2.3"
     *
     * \param icon  application icon name, e.g. "foobar"
     *
     * \version LibVLC 2.1.0 or later.
     */
    void setAppId(const std::string& id, const std::string& version, const std::string& icon);

    /**
     * Unsets the logging callback for a LibVLC instance. This is rarely
     * needed: the callback is implicitly unset when the instance is
     * destroyed. This function will wait for any pending callbacks
     * invocation to complete (causing a deadlock if called from within the
     * callback).
     *
     * \version LibVLC 2.1.0 or later
     */
    void logUnset();

    /**
     * Sets the logging callback for a LibVLC instance. This function is
     * thread-safe: it will wait for any pending callbacks invocation to
     * complete.
     *
     * \param data  opaque data pointer for the callback function
     *
     * \note Some log messages (especially debug) are emitted by LibVLC while
     * is being initialized. These messages cannot be captured with this
     * interface.
     *
     * \warning A deadlock may occur if this function is called from the
     * callback.
     *
     * \param p_instance  libvlc instance
     *
     * \version LibVLC 2.1.0 or later
     */
    void logSet(libvlc_log_cb cb, void * data);

    /**
     * Sets up logging to a file.
     *
     * \param stream  FILE pointer opened for writing (the FILE pointer must
     * remain valid until Instance::logUnset() )
     *
     * \version LibVLC 2.1.0 or later
     */
    void logSetFile(FILE * stream);

    /**
     * Returns a list of audio filters that are available.
     *
     * \return a list of module descriptions. It should be freed with
     * ModuleDescription::moduleDescriptionListRelease() . In case of an
     * error, NULL is returned.
     *
     * \see ModuleDescription
     *
     * \see ModuleDescription::moduleDescriptionListRelease()
     */
    std::vector<ModuleDescription> audioFilterList();

    /**
     * Returns a list of video filters that are available.
     *
     * \return a list of module descriptions. It should be freed with
     * ModuleDescription::moduleDescriptionListRelease() . In case of an
     * error, NULL is returned.
     *
     * \see ModuleDescription
     *
     * \see ModuleDescription::moduleDescriptionListRelease()
     */
    std::vector<ModuleDescription> videoFilterList();

    /**
     * Gets the list of available audio output modules.
     *
     * \return list of available audio outputs. It must be freed it with
     *
     * \see AudioOutputDescription::audioOutputListRelease()
     *
     * \see AudioOutputDescription . In case of error, NULL is returned.
     */
    std::vector<AudioOutputDescription> audioOutputList();

    /**
     * Gets a list of audio output devices for a given audio output module,
     *
     * \see Audio::outputDeviceSet() .
     *
     * \note Not all audio outputs support this. In particular, an empty
     * (NULL) list of devices does imply that the specified audio output does
     * not work.
     *
     * \note The list might not be exhaustive.
     *
     * \warning Some audio output devices in the list might not actually work
     * in some circumstances. By default, it is recommended to not specify
     * any explicit audio device.
     *
     * \param psz_aout  audio output name (as returned by
     * Instance::audioOutputList() )
     *
     * \return A vector containing all audio output devices for this module
     *
     * \version LibVLC 2.1.0 or later.
     */
    std::vector<AudioOutputDeviceDescription> audioOutputDeviceList(const std::string& aout);

private:
    explicit Instance( InternalPtr ptr, bool increaseRefCount );
    /**
     * Decrement the reference count of a libvlc instance, and destroy it if
     * it reaches zero.
     */
    void release();

    /**
     * Increments the reference count of a libvlc instance. The initial
     * reference count is 1 after Instance::Instance() returns.
     */
    void retain();
};

} // namespace VLC

#endif

