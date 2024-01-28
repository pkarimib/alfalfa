/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

/* Copyright 2013-2018 the Alfalfa authors
                       and the Massachusetts Institute of Technology

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:

      1. Redistributions of source code must retain the above copyright
         notice, this list of conditions and the following disclaimer.

      2. Redistributions in binary form must reproduce the above copyright
         notice, this list of conditions and the following disclaimer in the
         documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#ifndef PACER_HH
#define PACER_HH

#include <deque>
#include <chrono>
#include <string>

#include "packet.hh"

/* pace outgoing packets */
class Pacer
{
public:
  struct ScheduledPacket {
    std::chrono::system_clock::time_point scheduled_at; /* time that it entered the pacer */
    std::chrono::system_clock::time_point when; /* scheduled outgoing time of packet */
    std::string what; /* serialized packet contents */

    // added for Copa
    uint32_t frame_no;
    uint16_t fragment_no;
    size_t fragments_in_this_frame;

    std::chrono::steady_clock::time_point capture_time; /* time that the FRAME was captured */
  };

  std::deque<ScheduledPacket> queue_ {};

public:
  int ms_until_due() const
  {
    if ( queue_.empty() ) {
      return 1000; /* could be infinite, but if there's a bug I'd rather we find it in the first second */
    }

    int millis = std::chrono::duration_cast<std::chrono::milliseconds>( queue_.front().when - std::chrono::system_clock::now() ).count();
    if ( millis < 0 ) {
      millis = 0;
    }

    return millis;
  }

  bool empty() const { return queue_.empty(); }
  void push( const Packet& packet, const int delay_microseconds,
             const std::chrono::steady_clock::time_point capture_time )
  {
    const auto now = std::chrono::system_clock::now();

    if ( empty() ) {
      queue_.push_back( { now, now, packet.to_string(), packet.frame_no(), packet.fragment_no(),
                          packet.fragments_in_this_frame(), capture_time  } );
    } else {
      queue_.push_back( { now,
                          queue_.back().when + std::chrono::microseconds( delay_microseconds ),
                          packet.to_string(),
                          packet.frame_no(),
                          packet.fragment_no(),
                          packet.fragments_in_this_frame(),
                          capture_time } );
    }
  }

  const ScheduledPacket & front() const { return queue_.front(); }
  void pop() { queue_.pop_front(); }
  size_t size() const { return queue_.size(); }
};

#endif /* PACER_HH */
