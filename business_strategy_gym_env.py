import bss_module_december_twentysixth  # Not sure why there is an import error here. The module works.
import gymnasium as gym
import numpy as np
from gymnasium import spaces
from stable_baselines3 import PPO


class BusinessStrategyEnv(gym.Env):
    # TODO: get num_markets and num_agents from the config file and make the path to the config file dynamic
    def __init__(self, num_markets, num_agents):
        super().__init__()
        # INITIALIZE THE PYTHON API AND THE SIMULATOR
        self.python_API = bss_module_december_twentysixth.PythonAPI()
        iErr = self.python_API.init_simulator(
            '/Users/eric/CLionProjects/BusinessStrategy2.0/WorkingFiles/Config/default.json')
        if iErr == 0:
            print("The simulator initialized successfully.")
        else:
            print(f"The simulator failed to initialize correctly. Error code: {iErr}")

        # DEFINE THE ACTION SPACE
        self.action_space = spaces.Discrete(num_markets+1)

        # DEFINE THE OBSERVATION SPACE
        self.num_agents = num_agents
        self.num_markets = num_markets

        # Capital of all agents
        obs_low = np.zeros(self.num_agents)
        obs_high = np.full(self.num_agents, np.finfo(np.float32).max)

        # Market overlap structure
        obs_low = np.append(obs_low, np.zeros(self.num_markets**2))
        obs_high = np.append(obs_high, np.ones(self.num_markets**2))

        # Variable costs for all firm-market combinations
        obs_low = np.append(obs_low, np.zeros(self.num_agents * self.num_markets))
        obs_high = np.append(obs_high, np.full(self.num_agents * self.num_markets, np.finfo(np.float32).max))

        # Fixed cost for each firm-market combination
        obs_low = np.append(obs_low, np.zeros(self.num_agents * self.num_markets))
        obs_high = np.append(obs_high, np.full(self.num_agents * self.num_markets, np.finfo(np.float32).max))

        # Entry cost for every agent-firm combination
        obs_low = np.append(obs_low, np.zeros(self.num_agents * self.num_markets))
        obs_high = np.append(obs_high, np.full(self.num_agents * self.num_markets, np.finfo(np.float32).max))

        # Demand intercept and slope in each market
        obs_low = np.append(obs_low, np.zeros(2 * self.num_markets))
        obs_high = np.append(obs_high, np.full(2 * self.num_markets, np.finfo(np.float32).max))

        # Most recent quantity and price for each firm-market combination
        obs_low = np.append(obs_low, np.zeros(2 * self.num_agents * self.num_markets))
        obs_high = np.append(obs_high, np.full(2 * self.num_agents * self.num_markets, np.finfo(np.float32).max))

        obs_space = spaces.Box(low=obs_low, high=obs_high, dtype=np.float32)
        self.observation_space = obs_space

    def reset(self, seed=None, options=None):
        return self.python_API.reset(), {}
        # return observation, info

    def step(self, action):
        return self.python_API.step(action)
        # return observation, reward, terminated, truncated, info

    def render(self):
        pass

    def close(self):
        self.python_API.close()


# Define and train an RL agent
env = BusinessStrategyEnv(num_markets=6, num_agents=4)
model = PPO("MlpPolicy", env).learn(total_timesteps=10)
