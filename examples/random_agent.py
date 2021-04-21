#!/usr/bin/env python3
"""
Random example agent.

Samples a random action for one episode.
"""

import gym
import veins_gym


def main():
    """Run the random agent."""

    gym.register(
        id="veins-v1",
        entry_point="veins_gym:VeinsEnv",
        kwargs={
            "scenario_dir": "../scenario",
        },
    )

    env = gym.make("veins-v1")

    env.reset()
    done = False
    rewards = []
    while not done:
        random_action = env.action_space.sample()
        observation, reward, done, info = env.step(random_action)
        rewards.append(reward[0])  # note: reward is a 1-dimensional vector
    print("Number of steps taken:", len(rewards))
    print("Mean reward:", sum(rewards) / len(rewards))


if __name__ == "__main__":
    main()
