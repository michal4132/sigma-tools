export interface NetworkSettings {
  hostname: string;
  dhcp: boolean;
  ip: string;
  netmask: string;
  gateway: string;
  dns: string[];
}

export interface ZigbeeSettings {
  enabled: boolean;
  channel: number;
  pan_id: string;
  permit_join: boolean;
}

export interface MatterSettings {
  enabled: boolean;
  fabric_id: string;
  commission_mode: boolean;
}

export interface HardwareSettings {
  status_led: boolean;
  network_led: boolean;
}

export interface SystemSettings {
  name: string;
  timezone: string;
  log_level: 'debug' | 'info' | 'warn' | 'error';
  ssh_enabled: boolean;
  ssh_port: number;
}

export interface Settings {
  network: NetworkSettings;
  zigbee: ZigbeeSettings;
  matter: MatterSettings;
  hardware: HardwareSettings;
  system: SystemSettings;
}
