import { Settings } from '../types/settings';
import { apiRequest } from './client';

// Use mock data in development
const MOCK_MODE = true;

// Helper function to validate IP address
export function isValidIpAddress(ip: string): boolean {
  const ipv4Regex = /^(\d{1,3}\.){3}\d{1,3}$/;
  if (!ipv4Regex.test(ip)) return false;
  
  const parts = ip.split('.').map(part => parseInt(part, 10));
  return parts.every(part => part >= 0 && part <= 255);
}

// Helper function to validate hostname
export function isValidHostname(hostname: string): boolean {
  const hostnameRegex = /^[a-zA-Z0-9-]{1,63}$/;
  return hostnameRegex.test(hostname);
}

// Get all settings
export async function fetchSettings(): Promise<Settings> {
  return apiRequest<Settings>(
    MOCK_MODE ? '/mocks/settings.json' : '/settings'
  );
}

// Update settings by category
export async function updateSettings(
  category: keyof Settings,
  settings: Partial<Settings[keyof Settings]>
): Promise<void> {
  return apiRequest<void>(
    MOCK_MODE ? '/mocks/settings.json' : '/settings',
    {
      method: 'PATCH',
      body: { [category]: settings }
    }
  );
}
