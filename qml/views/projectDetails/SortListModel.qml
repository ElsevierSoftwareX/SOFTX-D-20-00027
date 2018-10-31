/*
 * Celltracker – A curation tool for object tracks.
 * Copyright (C) 2016 Sebastian Wagner
 *
 * Celltracker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Celltracker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Celltracker.  If not, see <https://www.gnu.org/licenses/>.
 */
import QtQuick 2.0

ListModel {
    property string sortColumnName: ""
    property int sortOrder: Qt.AscendingOrder
    id: sortlistmodel

    function swap(a, b) {
        if (a < b) {
            move(a, b, 1);
            move (b-1, a, 1);
        }
        else if (a > b) {
            move(b, a, 1);
            move (a-1, b, 1);
        }
    }

    function partition(begin, end, pivot)
    {
        var piv = get(pivot)[sortColumnName];
        swap(pivot, end-1);
        var store = begin;
        var ix;
        for (ix = begin; ix < end-1; ++ix) {
            if (sortOrder === Qt.AscendingOrder){
                if (get(ix)[sortColumnName] < piv) {
                    swap(store,ix);
                    ++store;
                }
            } else if (sortOrder === Qt.DescendingOrder){
                if (get(ix)[sortColumnName] > piv) {
                    swap(store,ix);
                    ++store;
                }
            }
        }
        swap(end-1, store);

        return store;
    }

    function qsort(begin, end)
    {
        if (end-1 > begin) {
            var pivot = begin + Math.floor(Math.random() * (end - begin));

            pivot = partition(begin, end, pivot);

            qsort(begin, pivot);
            qsort(pivot+1, end);
        }
    }

    function sort() {
        qsort(0, count)
    }

}

