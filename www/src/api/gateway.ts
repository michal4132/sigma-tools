import { GatewayData } from '../types/gateway';
import { apiRequest } from './client';

// Use mock data in development
const MOCK_MODE = true;

export async function fetchGatewayData(): Promise<GatewayData> {
  return apiRequest<GatewayData>(
    MOCK_MODE ? '/mocks/gateway-status.json' : '/gateway/status'
  );
}
