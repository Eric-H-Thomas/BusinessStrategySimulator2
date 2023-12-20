import bss_module_december_sixth # Not sure why there is an import error here. The module works.
import gymnasium as gym
import numpy as np
from gymnasium import spaces


class BusinessStrategyEnv(gym.Env):
    def __init__(self, num_markets, num_agents):
        super().__init__()
        # DEFINE THE ACTION SPACE
        self.action_space = spaces.Discrete(num_markets+1)

        # DEFINE THE OBSERVATION SPACE
        self.num_agents = num_agents
        self.num_markets = num_markets

        obs_low = np.zeros(self.num_agents)  # Capital of all agents
        obs_high = np.finfo(np.float32).max  # Assuming capital can be any positive value

        # Market overlap structure
        obs_low = np.append(obs_low, np.zeros(self.num_markets**2))
        obs_high = np.append(obs_high, np.ones(self.num_markets**2))

        # Variable costs for all firm-market combinations
        obs_low = np.append(obs_low, np.zeros(self.num_agents * self.num_markets))
        obs_high = np.append(obs_high, np.finfo(np.float32).max)

        # Fixed cost for each firm-market combination
        obs_low = np.append(obs_low, np.zeros(self.num_agents * self.num_markets))
        obs_high = np.append(obs_high, np.finfo(np.float32).max)

        # Entry cost for every agent-firm combination
        obs_low = np.append(obs_low, np.zeros(self.num_agents * self.num_markets))
        obs_high = np.append(obs_high, np.finfo(np.float32).max)

        # Demand intercept and slope in each market
        obs_low = np.append(obs_low, np.zeros(2 * self.num_markets))
        obs_high = np.append(obs_high, np.finfo(np.float32).max)

        # Most recent quantity and price for each firm-market combination
        obs_low = np.append(obs_low, np.zeros(2 * self.num_agents * self.num_markets))
        obs_high = np.append(obs_high, np.finfo(np.float32).max)

        obs_space = spaces.Box(low=obs_low, high=obs_high, dtype=np.float32)
        self.observation_space = obs_space


    def step(self, action):
        ...
        # return observation, reward, terminated, truncated, info

    def reset(self, seed=None, options=None):
        ...
        # return observation, info

    def render(self):
        ...

    def close(self):
        ...


python_API = bss_module_december_sixth.PythonAPI()

iErr = python_API.run_bss2_simulator(
    '/Users/eric/CLionProjects/BusinessStrategy2.0/WorkingFiles/Config/default.json')

if iErr == 0:
    print("The simulator finished processing successfully.")
else:
    print(f"The simulator finished processing with error code {iErr}")
