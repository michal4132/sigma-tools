// API configuration
export const API_CONFIG = {
  // Default to localhost for development
  baseUrl: import.meta.env.VITE_API_URL || 'http://localhost:8080/api',
  // Add version prefix to all API calls
  version: 'v1',
};

// Construct the full API URL
export const getApiUrl = (path: string): string => {
  return `${API_CONFIG.baseUrl}/${API_CONFIG.version}${path}`;
};
