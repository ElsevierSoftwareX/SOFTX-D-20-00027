/*
 * TraCurate – A curation tool for object tracks.
 * Copyright (C) 2016, 2015 Sebastian Wagner
 *
 * TraCurate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TraCurate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with TraCurate.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <iostream>
#include "idprovider.h"

#include <QDebug>
namespace TraCurate {

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
std::set<uint32_t> IdProvider::usedTrackletIds = std::set<uint32_t>();
std::set<uint32_t> IdProvider::usedAnnotationIds = std::set<uint32_t>();
std::set<uint32_t> IdProvider::usedAutoTrackletIds = std::set<uint32_t>();
#pragma clang diagnostic pop

uint32_t IdProvider::minFreeTrackletId = 0;
uint32_t IdProvider::minFreeAnnotationId = 0;
uint32_t IdProvider::minFreeAutoTrackletId = 0;

/*!
 * \brief returns an unused ID for use in a Tracklet
 * \return an unused ID
 */
uint32_t IdProvider::getNewTrackletId()
{
    uint32_t i, ret;

    ret = minFreeTrackletId;
    usedTrackletIds.insert(ret);

    /* find next free number */
    for (i = minFreeTrackletId;; i++) {
        if (usedTrackletIds.find(i) == usedTrackletIds.end()) {
            minFreeTrackletId = i;
            break;
        }
    }

    return ret;
}

/*!
 * \brief gives back the given ID, so it can be reused
 * \param id the ID to give back
 */
void IdProvider::returnTrackletId(uint32_t id)
{
    usedTrackletIds.erase(id);
    if (id < minFreeTrackletId)
        minFreeTrackletId = id;
}

/*!
 * \brief returns an unused ID for use in an Annotation
 * \return an unused ID
 */
uint32_t IdProvider::getNewAnnotationId()
{
    uint32_t i, ret;

    ret = minFreeAnnotationId;
    usedAnnotationIds.insert(ret);

    /* find next free number */
    for (i = minFreeAnnotationId;; i++) {
        if (usedAnnotationIds.find(i) == usedAnnotationIds.end()) {
            minFreeAnnotationId = i;
            break;
        }
    }

    return ret;
}

/*!
 * \brief tries to reserve a given ID
 * \param id the ID to reserve
 * \return true if reserving the ID succeeded, false otherwise
 *
 * \warning This may fail! Check the return value to see if the ID could be reserved
 */
bool IdProvider::claimAnnotationId(uint32_t id)
{
    uint32_t i, ret;

    if (usedAnnotationIds.find(id) == usedAnnotationIds.end()) {
        /* not yet in set */
        ret = id;
        usedAnnotationIds.insert(id);
        if (ret == minFreeAnnotationId) {
            for (i = minFreeAnnotationId;; i++) {
                if (usedAnnotationIds.find(i) == usedAnnotationIds.end()) {
                    minFreeAnnotationId = i;
                    break;
                }
            }
        }
        return true;
    } else {
        return false;
    }
}

/*!
 * \brief gives back the given ID, so it can be reused
 * \param id the ID to give back
 */
void IdProvider::returnAnnotationId(uint32_t id)
{
    usedAnnotationIds.erase(id);
    if (id < minFreeAnnotationId)
        minFreeAnnotationId = id;
}

/*!
 * \brief returns an unused ID for use in an AutoTracklet
 * \return an unused ID
 */
uint32_t IdProvider::getNewAutoTrackletId()
{
    uint32_t i, ret;

    ret = minFreeAutoTrackletId;
    usedAutoTrackletIds.insert(ret);

    /* find next free number */
    for (i = minFreeAutoTrackletId;; i++) {
        if (usedAutoTrackletIds.find(i) == usedAutoTrackletIds.end()) {
            minFreeAutoTrackletId = i;
            break;
        }
    }

    return ret;
}

/*!
 * \brief tries to reserve a given ID
 * \param id the ID to reserve
 * \return true if reserving the ID succeeded, false otherwise
 *
 * \warning This may fail! Check the return value to see if the ID could be reserved
 */
bool IdProvider::claimAutoTrackletId(uint32_t id)
{
    uint32_t i, ret;

    if (usedAutoTrackletIds.find(id) == usedAutoTrackletIds.end()) {
        /* not yet in set */
        ret = id;
        usedAutoTrackletIds.insert(id);
        if (ret == minFreeAutoTrackletId) {
            for (i = minFreeAutoTrackletId;; i++) {
                if (usedAutoTrackletIds.find(i) == usedAutoTrackletIds.end()) {
                    minFreeAutoTrackletId = i;
                    break;
                }
            }
        }
        return true;
    } else {
        return false;
    }
}

/*!
 * \brief gives back the given ID, so it can be reused
 * \param id the ID to give back
 */
void IdProvider::returnAutoTrackletId(uint32_t id)
{
    usedAutoTrackletIds.erase(id);
    if (id < minFreeAutoTrackletId)
        minFreeAutoTrackletId = id;
}

}
