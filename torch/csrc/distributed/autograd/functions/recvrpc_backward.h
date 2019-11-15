#pragma once

#include <torch/csrc/autograd/function.h>
#include <torch/csrc/distributed/autograd/context/context.h>
#include <torch/csrc/distributed/autograd/rpc_messages/autograd_metadata.h>
#include <torch/csrc/distributed/rpc/rpc_agent.h>

namespace torch {
namespace distributed {
namespace autograd {

class DistAutogradContext;

// As part of our distributed autograd implementation, whenever we receive an
// RPC from a node, we add a 'RecvRpcBackward' autograd function to the
// autograd graph. This is more or less a placeholder function that is used to
// pass gradients to the remote host during the backward pass. The inputs to the
// RPC function are the inputs to this autograd function.
class TORCH_API RecvRpcBackward : public torch::autograd::Node {
 public:
  explicit RecvRpcBackward(
      const AutogradMetadata& autogradMetadata,
      DistAutogradContext& autogradContext,
      rpc::worker_id_t fromWorkerId);

  torch::autograd::variable_list apply(
      torch::autograd::variable_list&& grads) override;

 private:
  const AutogradMetadata autogradMetadata_;

  // Hold a reference to the autograd context.
  DistAutogradContext& autogradContext_;

  // The worker id from which the RPC was received. During the backward pass,
  // we need to propagate the gradients to this workerId.
  rpc::worker_id_t fromWorkerId_;
};

} // namespace autograd
} // namespace distributed
} // namespace torch
