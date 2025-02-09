import { Log } from '../types/logs';
import { apiRequest } from './client';

interface LogsResponse {
  logs: Log[];
}

export async function fetchLogs(): Promise<Log[]> {
  const response = await apiRequest<LogsResponse>('/api/logs');
  return response.logs;
}
