import bss_module_december_twentysixth
import gymnasium as gym
import numpy as np
from gymnasium import spaces
from stable_baselines3 import PPO
from stable_baselines3.common.env_util import make_vec_env
from stable_baselines3.common.vec_env import SubprocVecEnv

num_envs = 2

class BusinessStrategyEnv(gym.Env):
    def __init__(self, path_to_config_file):
        super().__init__()
        # INITIALIZE THE PYTHON API AND THE SIMULATOR
        self.python_API = bss_module_december_twentysixth.PythonAPI()
        iErr = self.python_API.init_simulator(path_to_config_file)
        if iErr != 0:
            print(f"The simulator failed to initialize correctly. Error code: {iErr}")

        num_agents = self.python_API.get_num_agents()
        num_markets = self.python_API.get_num_markets()

        # DEFINE THE ACTION SPACE
        self.action_space = spaces.Discrete(num_markets+1)

        # DEFINE THE OBSERVATION SPACE
        # Capital of all agents
        obs_low = np.zeros(num_agents)
        obs_high = np.full(num_agents, np.finfo(np.float32).max)

        # Market overlap structure
        obs_low = np.append(obs_low, np.zeros(num_markets**2))
        obs_high = np.append(obs_high, np.ones(num_markets**2))

        # Variable costs for all firm-market combinations
        obs_low = np.append(obs_low, np.zeros(num_agents * num_markets))
        obs_high = np.append(obs_high, np.full(num_agents * num_markets, np.finfo(np.float32).max))

        # Fixed cost for each firm-market combination
        obs_low = np.append(obs_low, np.zeros(num_agents * num_markets))
        obs_high = np.append(obs_high, np.full(num_agents * num_markets, np.finfo(np.float32).max))

        # Market portfolio of all firms
        obs_low = np.append(obs_low, np.zeros(num_agents * num_markets))
        obs_high = np.append(obs_high, np.ones(num_agents * num_markets))

        # Entry cost for every agent-firm combination
        obs_low = np.append(obs_low, np.zeros(num_agents * num_markets))
        obs_high = np.append(obs_high, np.full(num_agents * num_markets, np.finfo(np.float32).max))

        # Demand intercept and slope in each market
        obs_low = np.append(obs_low, np.zeros(2 * num_markets))
        obs_high = np.append(obs_high, np.full(2 * num_markets, np.finfo(np.float32).max))

        # Most recent quantity and price for each firm-market combination
        obs_low = np.append(obs_low, np.zeros(2 * num_agents * num_markets))
        obs_high = np.append(obs_high, np.full(2 * num_agents * num_markets, np.finfo(np.float32).max))

        obs_space = spaces.Box(low=obs_low, high=obs_high, dtype=np.float32)
        self.observation_space = obs_space

    def reset(self, seed=None, options=None):
        observation, info = self.python_API.reset(), {}
        return observation, info

    def step(self, action):
        observation, reward, terminated, truncated = self.python_API.step(action)
        info = {}
        return observation, reward, terminated, truncated, info

    def render(self):
        pass

    def close(self):
        self.python_API.close()


def make_env(path_to_config_file):
    def _init():
        return BusinessStrategyEnv(path_to_config_file)
    return _init

# # Define and train an RL agent
# n_steps = 2048  # StableBaselines3 default value
# num_updates = 1
# total_steps = n_steps * n_envs * num_updates


if __name__ == '__main__':
    path_to_default_config_file = '/Users/eric/CLionProjects/BusinessStrategy2.0/WorkingFiles/Config/default.json'
    vec_env = make_vec_env(make_env(path_to_default_config_file), n_envs=num_envs, vec_env_cls=SubprocVecEnv)
    model = PPO("MlpPolicy", vec_env, verbose=1)
    model.learn(total_timesteps=2048)
    model.save("/Users/eric/CLionProjects/BusinessStrategy2.0/AgentFiles/Agent.zip")
