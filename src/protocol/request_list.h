// libTorrent - BitTorrent library
// Copyright (C) 2005-2011, Jari Sundell
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// In addition, as a special exception, the copyright holders give
// permission to link the code of portions of this program with the
// OpenSSL library under certain conditions as described in each
// individual source file, and distribute linked combinations
// including the two.
//
// You must obey the GNU General Public License in all respects for
// all of the code used other than OpenSSL.  If you modify file(s)
// with this exception, you may extend this exception to your version
// of the file(s), but you are not obligated to do so.  If you do not
// wish to do so, delete this exception statement from your version.
// If you delete this exception statement from all source files in the
// program, then also delete it here.
//
// Contact:  Jari Sundell <jaris@ifi.uio.no>
//
//           Skomakerveien 33
//           3185 Skoppum, NORWAY

#ifndef LIBTORRENT_REQUEST_LIST_H
#define LIBTORRENT_REQUEST_LIST_H

#include <deque>

#include "torrent/data/block_transfer.h"

namespace torrent {

class PeerChunks;
class Delegator;

class RequestList {
public:
  typedef std::deque<BlockTransfer*> ReserveeList;

  RequestList() :
    m_delegator(NULL),
    m_peerChunks(NULL),
    m_transfer(NULL),
    m_affinity(-1) {}

  // Some parameters here, like how fast we are downloading and stuff
  // when we start considering those.
  const Piece*         delegate();

  // If is downloading, call skip before cancel.
  void                 cancel();
  void                 stall();

  void                 clear();

  // The returned transfer must still be valid.
  bool                 downloading(const Piece& piece);

  void                 finished();
  void                 skipped();

  void                 transfer_dissimilar();

//   void                 cancel_transfer(BlockTransfer* transfer);

  bool                 is_downloading()                  { return m_transfer != NULL; }
  bool                 is_interested_in_active() const;

  bool                 has_index(uint32_t i);

  bool                 queued_empty() const               { return m_queued.empty(); }
  size_t               queued_size() const                { return m_queued.size(); }

  const Piece&         next_queued_piece() const          { return m_queued.front()->piece(); }

  bool                 canceled_empty() const             { return m_canceled.empty(); }
  size_t               canceled_size() const              { return m_queued.size(); }

  uint32_t             calculate_pipe_size(uint32_t rate);

  void                 set_delegator(Delegator* d)       { m_delegator = d; }
  void                 set_peer_chunks(PeerChunks* b)    { m_peerChunks = b; }

  BlockTransfer*       transfer()                        { return m_transfer; }
  const BlockTransfer* transfer() const                  { return m_transfer; }

  const BlockTransfer* queued_transfer(uint32_t i) const { return m_queued[i]; }

private:
  void                 cancel_range(ReserveeList::iterator end);

  inline BlockTransfer* pop_front_queued();

  inline void          push_back_queued(BlockTransfer* r);
  inline void          push_back_canceled(BlockTransfer* r);

  inline void          release_queued_range(ReserveeList::iterator begin, ReserveeList::iterator end);
  inline void          release_canceled_range(ReserveeList::iterator begin, ReserveeList::iterator end);
  inline void          move_to_canceled_range(ReserveeList::iterator begin, ReserveeList::iterator end);

  inline void          move_queued_to_transferring(ReserveeList::iterator itr);
  inline void          move_canceled_to_transferring(ReserveeList::iterator itr);

  Delegator*           m_delegator;
  PeerChunks*          m_peerChunks;

  BlockTransfer*       m_transfer;

  // Replace m_downloading with a pointer to BlockTransfer.
  int32_t              m_affinity;

  ReserveeList         m_queued;
  ReserveeList         m_canceled;
};

}

#endif
