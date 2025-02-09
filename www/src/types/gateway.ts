export interface GatewayStatus {
  status: string;
  uptime: string;
  ip: string;
  version: string;
}

export interface NetworkStatus {
  zigbee: {
    status: string;
    signal_strength: number;
    channel: number;
    pan_id: string;
  };
  matter: {
    status: string;
    fabric_id: string;
  };
}

export interface Device {
  id: string;
  name: string;
  type: string;
  protocol: 'zigbee' | 'matter';
  status: 'online' | 'offline';
  battery?: number;
  last_seen: string;
}

export interface GatewayData {
  gateway: GatewayStatus;
  network: NetworkStatus;
  devices: Device[];
}
