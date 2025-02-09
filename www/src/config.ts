// Global mock mode setting
export const MOCK_MODE = false;

// API configuration
export const API_CONFIG = {
  // Empty base URL since we include full paths in API calls
  baseUrl: '',
  // No version prefix needed
  version: '',
};

// Construct the full API URL
export const getApiUrl = (path: string): string => {
  if (MOCK_MODE) {
    return `/mocks${path}.json`;
  }
  return path;
};
