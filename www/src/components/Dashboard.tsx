import { fetchGatewayData } from '../api/gateway'
import { Device, GatewayStatus, GatewayData } from '../types/gateway'
import { Card } from './shared/Card'
import { StatRow } from './shared/StatRow'
import { PageHeader } from './shared/PageHeader'
import { LoadingButton } from './shared/LoadingButton'
import { SkeletonCard } from './shared/Skeleton'
import { ErrorAlert } from './shared/ErrorAlert'
import { useAsyncData } from '../hooks/useAsyncData'

interface Props {
  path?: string
  MenuButton: () => JSX.Element
}

interface NetworkData {
  zigbee: {
    signal_strength: number
    channel: number
    pan_id: string
  }
}

interface StatItem {
  id: string
  name: string
  value: string | number
  type: 'status' | 'info'
}

const GATEWAY_STATS = {
  status: 'Gateway Status',
  uptime: 'Uptime',
  ip: 'IP Address'
} as const

const DEVICE_STATS = {
  total: 'Total Devices',
  zigbee: 'Zigbee Devices',
  matter: 'Matter Devices'
} as const

function DashboardSkeleton() {
  return (
    <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6">
      <SkeletonCard title="Gateway Status" rows={3} />
      <SkeletonCard title="Network" rows={3} />
      <SkeletonCard title="Devices" rows={3} />
      <SkeletonCard 
        title="Connected Devices" 
        className="col-span-1 md:col-span-2 lg:col-span-3"
        layout="table"
        rows={4}
        columns={6}
      />
    </div>
  )
}

function getDeviceCounts(devices: Device[]) {
  return {
    total: devices.length,
    zigbee: devices.filter(d => d.protocol === 'zigbee').length,
    matter: devices.filter(d => d.protocol === 'matter').length
  }
}

function getGatewayStats(gateway: GatewayStatus): StatItem[] {
  return [
    { id: 'status', name: GATEWAY_STATS.status, value: gateway.status, type: 'status' },
    { id: 'uptime', name: GATEWAY_STATS.uptime, value: gateway.uptime, type: 'info' },
    { id: 'ip', name: GATEWAY_STATS.ip, value: gateway.ip, type: 'info' }
  ]
}

export function Dashboard({ MenuButton }: Props) {
  const { 
    data: gatewayData, 
    error, 
    loading,
    refreshing,
    refresh: loadGatewayData 
  } = useAsyncData(fetchGatewayData)

  const deviceCounts = gatewayData ? getDeviceCounts(gatewayData.devices) : { total: 0, zigbee: 0, matter: 0 }
  const stats = gatewayData ? getGatewayStats(gatewayData.gateway) : []

  const headerActions = (
    <LoadingButton
      onClick={loadGatewayData}
      loading={refreshing}
      loadingText="Refreshing..."
      text="Refresh"
    />
  )

  return (
    <div>
      <PageHeader 
        title="Dashboard" 
        MenuButton={MenuButton} 
        actions={headerActions} 
      />

      <ErrorAlert error={error} className="mb-4" />

      {loading && !gatewayData ? (
        <DashboardSkeleton />
      ) : gatewayData && (
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6">
          <Card title="Gateway Status">
            <div className="space-y-3">
              {stats.map(stat => (
                <StatRow key={stat.id} label={stat.name} value={stat.value} type={stat.type} />
              ))}
            </div>
          </Card>

          <Card title="Network">
            <div className="space-y-4">
              <div>
                <div className="flex justify-between mb-2">
                  <span className="text-text-secondary">Signal Strength</span>
                  <span className="text-primary">{gatewayData.network.zigbee.signal_strength}%</span>
                </div>
                <div className="w-full bg-bg-accent rounded-full h-2">
                  <div 
                    className="bg-primary h-2 rounded-full transition-all duration-500" 
                    style={{ width: `${gatewayData.network.zigbee.signal_strength}%` }}
                  ></div>
                </div>
              </div>
              <StatRow label="Channel" value={gatewayData.network.zigbee.channel} />
              <StatRow label="PAN ID" value={gatewayData.network.zigbee.pan_id} />
            </div>
          </Card>

          <Card title="Devices">
            <div className="space-y-3">
              {Object.entries(deviceCounts).map(([key, value]) => (
                <StatRow 
                  key={key} 
                  label={DEVICE_STATS[key as keyof typeof DEVICE_STATS]} 
                  value={value} 
                />
              ))}
            </div>
          </Card>

          <Card title="Connected Devices" className="col-span-1 md:col-span-2 lg:col-span-3">
            <div className="overflow-x-auto scrollbar scrollbar-thumb-bg-accent scrollbar-track-bg-secondary w-full">
              <table className="min-w-[800px] w-full">
                <thead>
                  <tr className="text-left text-text-secondary">
                    <th className="pb-4">Name</th>
                    <th className="pb-4">Type</th>
                    <th className="pb-4">Protocol</th>
                    <th className="pb-4">Status</th>
                    <th className="pb-4">Battery</th>
                    <th className="pb-4">Last Seen</th>
                  </tr>
                </thead>
                <tbody className="divide-y divide-bg-accent">
                  {gatewayData.devices.map((device) => (
                    <tr key={device.id} className="text-text-primary">
                      <td className="py-3">{device.name}</td>
                      <td className="py-3">{device.type}</td>
                      <td className="py-3 capitalize">{device.protocol}</td>
                      <td className="py-3">
                        <div className="flex items-center">
                          <div className={`h-2 w-2 rounded-full ${device.status === 'online' ? 'bg-success' : 'bg-warning'} mr-2`}></div>
                          <span className="capitalize">{device.status}</span>
                        </div>
                      </td>
                      <td className="py-3">{device.battery ? `${device.battery}%` : '-'}</td>
                      <td className="py-3">{device.last_seen}</td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>
          </Card>
        </div>
      )}
    </div>
  )
}
