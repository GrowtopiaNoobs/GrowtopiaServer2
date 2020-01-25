/**********************************************************************************
	New Growtopia Private Server
	Copyright (C) 2019  Growtopia Noobs
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Affero General Public License as published
	by the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Affero General Public License for more details.
	You should have received a copy of the GNU Affero General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
**********************************************************************************/


#pragma once

#include "Peer.hpp"

class PeersIterator { // https://gist.github.com/jeetsukumaran/307264

	ENetPeer* peersBegin = NULL;
	int size = 0;
	std::string serverID = "";

	class iterator
	{
	public:
		typedef iterator self_type;
		typedef ENetPeer* value_type;
		typedef ENetPeer& reference;
		typedef ENetPeer* pointer;
		typedef std::forward_iterator_tag iterator_category;
		typedef int difference_type;
		iterator(pointer ptr, pointer begin, int size, std::string serverID) : ptr_(ptr), serverID(std::move(serverID)) { begin_ = begin; size_ = size; }
		self_type operator++() {
			self_type i = *this;
			ptr_++;
			while (ptr_ < &begin_[size_] && ptr_->state != ENET_PEER_STATE_CONNECTED)
				ptr_++;
			return i;
		}
		self_type operator++(int junk) {
			ptr_++;
			while (ptr_ < &begin_[size_] && ptr_->state != ENET_PEER_STATE_CONNECTED)
				ptr_++;
			return *this;
		}
		Peer operator*() { Peer p(ptr_, serverID); return p; }
		Peer operator->() { Peer p(ptr_, serverID); return p; }
		bool operator==(const self_type& rhs) { return ptr_ == rhs.ptr_; }
		bool operator!=(const self_type& rhs) { return ptr_ != rhs.ptr_; }
	private:
		pointer ptr_;
		pointer begin_;
		int size_;
		std::string serverID = "";
	};
public:
	iterator begin() const
	{
		int i = 0;
		while (true)
		{
			if (peersBegin[i].state != ENET_PEER_STATE_CONNECTED && i<size)
			{
				i++;
			}
			else {
				break;
			}
		}

		return iterator(&peersBegin[i], peersBegin, size, serverID);
	}

	iterator end() const
	{
		return iterator(&peersBegin[size], peersBegin, 0, serverID);
	}

	PeersIterator(ENetPeer* peers, int size, std::string serverID) : serverID(std::move(serverID)) {
		this->peersBegin = peers;
		this->size = size;
	}

	PeersIterator() {
		this->peersBegin = 0;
		this->size = 0;
	}

};
