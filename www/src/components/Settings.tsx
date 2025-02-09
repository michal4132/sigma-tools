import { useState } from 'preact/hooks'
import { Settings as SettingsType, fetchSettings, updateSettings, isValidIpAddress, isValidHostname } from '../api/settings'
import { Card } from './shared/Card'
import { LoadingButton } from './shared/LoadingButton'
import { PageHeader } from './shared/PageHeader'
import { ErrorAlert } from './shared/ErrorAlert'
import { useAsyncData } from '../hooks/useAsyncData'
import { SkeletonCard } from './shared/Skeleton'

interface Props {
  path?: string
  MenuButton: () => JSX.Element
}

const CATEGORIES = {
  network: 'Network',
  zigbee: 'Zigbee',
  matter: 'Matter',
  hardware: 'Hardware',
  system: 'System'
} as const

const SETTING_LABELS: Record<string, string> = {
  ip: 'IP Address',
  dns_primary: 'Primary DNS',
  dns_secondary: 'Secondary DNS',
  dhcp: 'DHCP',
  ssh_enabled: 'SSH Access',
  ssh_port: 'SSH Port'
}

const DHCP_MANAGED_FIELDS = ['ip', 'netmask', 'gateway', 'dns_primary', 'dns_secondary']

function formatLabel(key: string): string {
  return SETTING_LABELS[key] || key.split('_').map(word => word.charAt(0).toUpperCase() + word.slice(1)).join(' ')
}

function SettingsSkeleton() {
  return (
    <div className="max-w-2xl mx-auto grid gap-6">
      {Object.entries(CATEGORIES).map(([key, name]) => (
        <SkeletonCard 
          key={key} 
          title={name} 
          rows={3} 
          layout="settings"
        />
      ))}
    </div>
  )
}

export function Settings({ MenuButton }: Props) {
  const { 
    data: settings, 
    error,
    setData: setSettings,
    loading
  } = useAsyncData(fetchSettings)
  
  const [modifiedSettings, setModifiedSettings] = useState<SettingsType | null>(null)
  const [saving, setSaving] = useState(false)
  const [saveError, setSaveError] = useState<string | null>(null)

  // Update modified settings when settings are loaded
  if (settings && !modifiedSettings) {
    setModifiedSettings(settings)
  }

  async function handleSaveAll() {
    if (!modifiedSettings || !settings) return

    try {
      setSaving(true)
      setSaveError(null)

      // Find changed categories
      const changedCategories = Object.keys(modifiedSettings).filter(category => {
        const categoryKey = category as keyof SettingsType
        return JSON.stringify(modifiedSettings[categoryKey]) !== JSON.stringify(settings[categoryKey])
      })

      // Update each changed category
      for (const category of changedCategories) {
        await updateSettings(
          category as keyof SettingsType,
          modifiedSettings[category as keyof SettingsType]
        )
      }

      // Update local settings after successful save
      setSettings(modifiedSettings)
    } catch (err) {
      setSaveError('Failed to save settings')
      console.error(err)
    } finally {
      setSaving(false)
    }
  }

  function handleSettingChange(
    category: keyof SettingsType,
    key: string,
    value: any
  ) {
    setModifiedSettings(prev => prev ? {
      ...prev,
      [category]: {
        ...prev[category],
        [key]: value
      }
    } : null)
  }

  const hasChanges = JSON.stringify(settings) !== JSON.stringify(modifiedSettings)

  const headerActions = (
    <LoadingButton
      onClick={handleSaveAll}
      loading={saving}
      loadingText="Saving..."
      text="Save Changes"
      disabled={!hasChanges}
    />
  )

  return (
    <div>
      <PageHeader title="Settings" MenuButton={MenuButton} actions={headerActions} />
      
      <ErrorAlert error={error || saveError} className="mb-4" />

      {loading ? (
        <SettingsSkeleton />
      ) : settings && modifiedSettings ? (
        <div className="max-w-2xl mx-auto grid gap-6">
          {Object.entries(CATEGORIES).map(([category, name]) => (
            <Card key={category} title={name}>
              <div className="space-y-4">
                {Object.entries(settings[category as keyof SettingsType] || {}).map(([key, value]) => {
                  // Skip DHCP-managed fields when DHCP is enabled
                  if (
                    category === 'network' && 
                    DHCP_MANAGED_FIELDS.includes(key) && 
                    modifiedSettings?.network?.dhcp
                  ) {
                    return null
                  }

                  return (
                    <div key={key}>
                      <div className="flex flex-row justify-between items-center gap-4">
                        <span className="text-text-secondary">{formatLabel(key)}</span>
                        {typeof value === 'boolean' ? (
                          <label className="relative inline-flex items-center cursor-pointer sm:self-center">
                            <input
                              type="checkbox"
                              checked={modifiedSettings[category as keyof SettingsType][key]}
                              onChange={(e) => handleSettingChange(
                                category as keyof SettingsType,
                                key,
                                (e.target as HTMLInputElement).checked
                              )}
                              className="sr-only peer"
                            />
                            <div className="w-11 h-6 bg-bg-accent peer-focus:outline-none peer-focus:ring-2 peer-focus:ring-primary rounded-full peer peer-checked:after:translate-x-full peer-checked:after:border-white after:content-[''] after:absolute after:top-[2px] after:left-[2px] after:bg-white after:border-gray-300 after:border after:rounded-full after:h-5 after:w-5 after:transition-all peer-checked:bg-primary"></div>
                          </label>
                        ) : (
                          <input
                            type="text"
                            value={modifiedSettings[category as keyof SettingsType][key]}
                            onChange={(e) => handleSettingChange(
                              category as keyof SettingsType,
                              key,
                              (e.target as HTMLInputElement).value
                            )}
                            className="bg-bg-accent text-text-primary px-3 py-2 rounded-md border border-bg-accent focus:border-primary focus:outline-none w-full sm:w-auto"
                          />
                        )}
                      </div>
                      {category === 'network' && key === 'dhcp' && modifiedSettings.network.dhcp && (
                        <div className="mt-2 text-sm text-text-secondary bg-bg-accent/50 p-3 rounded">
                          IP configuration is managed automatically by DHCP
                        </div>
                      )}
                    </div>
                  )
                })}
              </div>
            </Card>
          ))}
        </div>
      ) : null}
    </div>
  )
}
