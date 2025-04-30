import numpy as np

# Constants
INT_SCALE = 10000
CPU_COUNT = 12

# Function to normalize values
def normalize_values(arr, min_vals, max_vals):
    """
    Map arr[i] from [min_vals[i], max_vals[i]] => [-1,1] range
    """
    normalized = []
    for i in range(len(arr)):
        if max_vals[i] == min_vals[i]:
            norm = -1.0
        else:
            norm = 2.0 * ((arr[i] - min_vals[i]) / (max_vals[i] - min_vals[i])) - 1.0
        norm = np.clip(norm, -1.0, 1.0)
        normalized.append(norm)
    return np.array(normalized, dtype=np.float32)

# Function to convert float observations to int
def float_obs_to_int(obs: np.ndarray, scale: float = INT_SCALE) -> np.ndarray:
    """
    obs: shape=(obs_dim,) float array
    -> scaled int array
    Example: obs_float * 10000
    """
    obs_scaled = obs * scale
    obs_int = obs_scaled.astype(np.int32)
    return obs_int
