/** $lic$
 *
* This file is part of zsim.
 *
 * zsim is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * If you use this software in your research, we request that you reference
 * the zsim paper ("ZSim: Fast and Accurate Microarchitectural Simulation of
 * Thousand-Core Systems", Sanchez and Kozyrakis, ISCA-40, June 2013) as the
 * source of the simulator in any publications that use this software, and that
 * you send us a citation of your work.
 *
 * zsim is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

/*
This file contains the function declarations of both the best offset prefetcher
as well as the recent requests table implemenation.  
*/

//#define TEST

#ifndef BEST_OFFSET_PREFETCHER_H_
#define BEST_OFFSET_PREFETCHER_H_

#ifndef TEST 
#include "cache_prefetcher.h"
#include "filter_cache.h"
#include "g_std/g_string.h"
#include "memory_hierarchy.h"
#include "stats.h" 
#include "hash.h"
#else
#include <map>
#include <vector>
#include <utility>
#include <iostream>
#include <deque>
#include <cmath> 
#endif

#define SLIDING
#define SCANNING
//#define LIMIT_TO_PAGE
//#define IN_PAGE_OFFSETS
//#define THRASH_OFFSETS

// global constants. Note that these are either not configurable or should not be changed.
const uint64_t coherency_time = 19; 

#ifdef THRASH_OFFSETS
const uint64_t num_offsets = 4;
#else
#ifdef IN_PAGE_OFFSETS
const uint64_t num_offsets = 63;
#else
const uint64_t num_offsets = 52;
#endif
#endif

const uint64_t page_size = pow(2, 12);
const uint64_t page_mask = 0xFFFFFFFFFFFFFFFF - page_size + 1;

// declarations for the recent requests table implemenation
class RR {
    public:
        RR(); 
        void insert(uint64_t _addr, uint64_t _cycle);
        void clear();
        bool exists(uint64_t _addr, uint64_t _cycle);
        uint64_t size_ = 0;
        uint64_t max_size_;
    private:
        std::map<uint64_t, std::pair<uint64_t,uint64_t>> hash;
        std::deque<std::pair<uint64_t, uint64_t>> list;
        void deletion();
        uint64_t id = 0; 
};

#ifndef TEST
class BestOffsetPrefetcher : public CachePrefetcher {
public:
    explicit BestOffsetPrefetcher(const g_string& _name, const g_string& _target,
                                bool _monitor_GETS, bool _monitor_GETX,
                                uint32_t _degree,
                                uint64_t _round_max,
                                uint64_t _max_score,
                                uint64_t _init_offset,
                                uint64_t _target_latency_ ,
                                uint64_t rr_size
                                );
    void initStats(AggregateStat* _parentStat) override;
    uint64_t access(MemReq& _req) override;
    void prefetch(MemReq& _req) override;
#else
class BestOffsetPrefetcher {
public:
    explicit BestOffsetPrefetcher(uint64_t _round_max,
                                uint64_t _max_score,
                                uint64_t _init_offset,
                                uint64_t rr_size
                                );
#endif
    void learn(uint64_t _addr, uint64_t _cycle); 
    void resetPrefetcher();
private:
    // recent requests list object
    RR recent_requests_; 
    // functions
    void moveTestOffsetPtr();
    void resetOffsets();
    uint64_t findMaxScore();
    void printScores();
    // private vars
    bool monitor_GETS_;
    bool monitor_GETX_;
    uint32_t degree_;
    std::vector<std::pair<const uint64_t, uint64_t>> offset_scores_;
    uint64_t current_round_;
    uint64_t test_offset_index_;
    uint64_t current_offset_;
    const uint64_t round_max_;
    const uint64_t max_score_;
    const uint64_t init_offset_;
#ifndef TEST
    const uint64_t target_latency_;
#endif
    uint64_t total_phases_ = 0;
    uint64_t total_rounds_ = 0;
    uint64_t high_score_total_ = 0;
    float average_hits_per_round__ = 0; 
#ifndef TEST
    Counter prof_emitted_prefetches_, recent_requests_hits_, average_rounds_, average_hits_per_round_, all_time_max_score_, all_time_max_score_offset_, average_max_score_;
#endif
    std::vector<uint64_t> paper_offset_list {1,2,3,4,5,6,8,9,10,12,15,16,18,20,24,25,27,30,32,36,40,45,48,50,54,60,64,72,75,80,81,90,96,100,108,120,125,128,135,144,150,160,162,180,192,200,216,225,240,243,250,256}; 
};

#endif
