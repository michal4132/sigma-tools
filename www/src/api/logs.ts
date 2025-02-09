import { Log } from '../types/logs';
import { apiRequest } from './client';

// Use mock data in development
const MOCK_MODE = true;

interface LogsResponse {
  logs: Log[];
}

export async function fetchLogs(): Promise<Log[]> {
  const response = await apiRequest<LogsResponse>(
    MOCK_MODE ? '/mocks/logs.json' : '/logs'
  );
  return response.logs;
}
