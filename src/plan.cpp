

#include <hptt/compute_node.h>
#include <hptt/plan.h>
#include <hptt/utils.h>

namespace hptt {

Plan::Plan(std::vector<int> loopOrder, std::vector<int> numThreadsAtLoop)
    : rootNodes_(nullptr), loopOrder_(loopOrder),
      numThreadsAtLoop_(numThreadsAtLoop) {
  numTasks_ = 1;
  for (auto nt : numThreadsAtLoop)
    numTasks_ *= nt;
  rootNodes_ = new ComputeNode[numTasks_];
}

Plan::~Plan() {
  if (rootNodes_ != nullptr)
    delete[] rootNodes_;
}

const ComputeNode *Plan::getRootNode_const(int threadId) const {
  return &rootNodes_[threadId];
}
ComputeNode *Plan::getRootNode(int threadId) const {
  return &rootNodes_[threadId];
}

void Plan::print() const {
  printVector(loopOrder_, "LoopOrder");
  printVector(numThreadsAtLoop_, "Parallelization");
}
} // namespace hptt
