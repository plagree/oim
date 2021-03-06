/*
 Copyright (c) 2015 Siyu Lei, Silviu Maniu, Luyi Mo

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#ifndef __oim__PathSampler__
#define __oim__PathSampler__

#include <queue>
#include <iostream>
#include <unordered_set>
#include <random>
#include <sys/time.h>
#include <boost/heap/fibonacci_heap.hpp>

#include "common.hpp"
#include "Graph.hpp"
#include "Sampler.hpp"

using namespace std;

/**
  Makes false samples based on path estimations. The expected spread corresponds
  to the sum of expected activation of each node, computed using paths.
*/
class PathSampler : public Sampler {
 private:
  struct NodeType {
    unode_int id;
    double prob;
    bool operator<(const NodeType& a) const {
      return (prob < a.prob) ? true : ((prob > a.prob) ? false : id > a.id);
    }
  };

 public:
  PathSampler(unsigned int type, int model)
      : Sampler(type, model) {};

  double sample(const Graph& graph,
                const std::unordered_set<unode_int>& activated,
                const std::unordered_set<unode_int>& seeds,
                unode_int samples) {
    return perform_sample(graph, activated, seeds, samples, false);
  }

  double trial(const Graph& graph,
               const std::unordered_set<unode_int>& activated,
               const std::unordered_set<unode_int>& seeds, bool inv) {
    return perform_sample(graph, activated, seeds, 1, true, inv);
  }

  std::shared_ptr<vector<unode_int>> perform_unique_sample(
      const Graph&, vector<unode_int>&,
      vector<bool>&, const unode_int,
      const std::unordered_set<unode_int>&, bool) {
    return shared_ptr<vector<unode_int>>(NULL);
  }

  std::unordered_set<unode_int> perform_diffusion(
      const Graph&, const std::unordered_set<unode_int>&) {
    return std::unordered_set<unode_int>();
  }

private:
  double perform_sample(const Graph& graph,
                        const std::unordered_set<unode_int>& activated,
                        const std::unordered_set<unode_int>& seeds,
                        unode_int, bool trial, bool inv=false) {
    trials_.clear();
    boost::heap::fibonacci_heap<NodeType> queue;
    std::unordered_map<unode_int,
        boost::heap::fibonacci_heap<NodeType>::handle_type> queue_nodes;
    std::unordered_set<unode_int> visited;
    double spread = 0.0;
    for (unode_int seed : seeds) {
      NodeType node;
      node.id = seed;
      node.prob = 1.0;
      queue_nodes[seed] = queue.push(node);
      if (trial) {
        TrialType tt;
        tt.source = node.id;
        tt.target = node.id;
        tt.trial = 1;
        trials_.push_back(tt);
      }
    }
    while (queue.size() > 0) {
      NodeType node = queue.top();
      queue.pop();
      if(trial) {
        TrialType tt;
        tt.source = node.id;
        tt.target = node.id;
        tt.trial = 1;
        trials_.push_back(tt);
      }
      if (activated.find(node.id) == activated.end())
        spread += node.prob;
      if (node.prob < 0.001)
        break;
      visited.insert(node.id);
      sample_outgoing_edges(graph, node.id, queue, visited, queue_nodes, inv);
    }
    return spread;
  }

  void sample_outgoing_edges(
      const Graph& graph, unode_int node,
      boost::heap::fibonacci_heap<NodeType>& queue,
      std::unordered_set<unode_int>& visited,
      std::unordered_map<unode_int,
          boost::heap::fibonacci_heap<NodeType>::handle_type>& queue_nodes,
      bool inv=false) {

    if (graph.has_neighbours(node, inv)) {
      for (auto edge : graph.get_neighbours(node, inv)) {
        if (visited.find(edge.target) == visited.end()) {
          double dst_prob = edge.dist->sample(type_);
          relax(node, edge.target, dst_prob, queue, queue_nodes);
        }
      }
    }
  }

  void relax(
      unode_int src, unode_int tgt, double dst,
      boost::heap::fibonacci_heap<NodeType>& queue,
      std::unordered_map<unode_int,
          boost::heap::fibonacci_heap<NodeType>::handle_type>& queue_nodes) {

    double new_prob = (*queue_nodes[src]).prob * dst;
    if (queue_nodes.find(tgt) == queue_nodes.end()) {
      NodeType node;
      node.id = tgt;
      node.prob = new_prob;
      queue_nodes[tgt] = queue.push(node);
    } else {
      double prev_prob = (*queue_nodes[tgt]).prob;
      if (new_prob > prev_prob) {
        NodeType node;
        node.id = tgt;
        node.prob = new_prob;
        auto handle = queue_nodes[tgt];
        queue.update(handle, node);
      }
    }
  }
};

#endif /* defined(__oim__PathSampler__) */
