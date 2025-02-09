import { GatewayData } from '../types/gateway';
import { apiRequest } from './client';

export async function fetchGatewayData(): Promise<GatewayData> {
  return apiRequest<GatewayData>('/api/gateway/status');
}
