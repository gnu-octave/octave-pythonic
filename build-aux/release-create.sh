#!/bin/sh
#
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2019 Mike Miller
#
# This file is part of Octave Pythonic.
#
# Octave Pythonic is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Octave Pythonic is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Octave Pythonic; see the file COPYING.  If not, see
# <https://www.gnu.org/licenses/>.

dir=$(dirname "$0")
prog=$(basename "$0")
repo="https://octave-pythonic-downloads.s3.amazonaws.com/releases"

if [ x"$CI_API_V4_URL" = x ]; then
  echo >&2 "$prog: CI_API_V4_URL environment variable missing"
  exit 1
fi

if [ x"$CI_COMMIT_TAG" = x ]; then
  echo >&2 "$prog: CI_COMMIT_TAG environment variable missing"
  exit 1
fi

if [ x"$CI_JOB_TOKEN" = x ]; then
  echo >&2 "$prog: CI_JOB_TOKEN environment variable missing"
  exit 1
fi

if [ x"$CI_PROJECT_ID" = x ]; then
  echo >&2 "$prog: CI_PROJECT_ID environment variable missing"
  exit 1
fi

if ! echo "$CI_COMMIT_TAG" | grep -E '^v[0-9]+(\.[0-9]+)*' > /dev/null 2>&1; then
  echo >&2 "$prog: CI_COMMIT_TAG value does not match expected pattern"
  exit 1
fi

api_endpoint="projects/$CI_PROJECT_ID/releases"
version="$(echo $CI_COMMIT_TAG | sed 's/^v//')"
description="$($dir/release-description-gen.py "$version" < NEWS.md)"
release_tar="octave-pythonic-$version.tar.gz"
release_sig="octave-pythonic-$version.tar.gz.asc"

data="{
  \"name\": \"$version\",
  \"tag_name\": \"$CI_COMMIT_TAG\",
  \"description\": \"$description\",
  \"assets\": { \"links\": [
    { \"name\": \"$release_tar (package)\", \"url\": \"$repo/$release_tar\" },
    { \"name\": \"$release_sig (signature)\", \"url\": \"$repo/$release_sig\" }
  ] }
}"

curl --header "Content-Type: application/json" \
     --header "PRIVATE-TOKEN: $CI_JOB_TOKEN" \
     --data "$data" \
     --request POST "$CI_API_V4_URL/$api_endpoint"
