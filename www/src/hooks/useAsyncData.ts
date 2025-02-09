import { useState, useEffect, useCallback } from 'preact/hooks'

interface UseAsyncDataOptions<T> {
  initialData?: T
  onError?: (error: Error) => void
}

export function useAsyncData<T>(
  fetchFn: () => Promise<T>,
  options: UseAsyncDataOptions<T> = {}
) {
  const [data, setData] = useState<T | null>(options.initialData || null)
  const [loading, setLoading] = useState(true)
  const [error, setError] = useState<string | null>(null)
  const [refreshing, setRefreshing] = useState(false)

  const refresh = useCallback(async () => {
    try {
      setError(null)
      setRefreshing(true)
      const result = await fetchFn()
      setData(result)
    } catch (err) {
      const error = err as Error
      setError(error.message)
      options.onError?.(error)
      console.error(err)
    } finally {
      setLoading(false)
      setRefreshing(false)
    }
  }, [fetchFn, options.onError])

  useEffect(() => {
    refresh()
  }, [refresh])

  return {
    data,
    loading,
    error,
    refreshing,
    refresh,
    setData
  }
}
