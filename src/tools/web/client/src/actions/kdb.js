import { thunkCreator, encodePath } from './utils'

//~~~

export const GET_KEY_REQUEST = 'GET_KEY_REQUEST'
export const GET_KEY_SUCCESS = 'GET_KEY_SUCCESS'
export const GET_KEY_FAILURE = 'GET_KEY_FAILURE'

export const getKey = (id, path, cluster = false) => thunkCreator({
  types: [GET_KEY_REQUEST, GET_KEY_SUCCESS, GET_KEY_FAILURE],
  promise: fetch(`/${cluster ? 'clusters' : 'instances'}/${id}/kdb/${encodePath(path)}`)
    .then(response => response.json())
    .then(result => {
      return { ...result, id, path }
    }),
})

export const getClusterKey = (id, path, value) => getKey(id, path, true)

//~~~

export const SET_KEY_REQUEST = 'SET_KEY_REQUEST'
export const SET_KEY_SUCCESS = 'SET_KEY_SUCCESS'
export const SET_KEY_FAILURE = 'SET_KEY_FAILURE'

export const setKey = (id, path, value, cluster = false) => thunkCreator({
  request: { id, path, value }, // TODO: use this syntax everywhere
  types: [SET_KEY_REQUEST, SET_KEY_SUCCESS, SET_KEY_FAILURE],
  promise: fetch(`/${cluster ? 'clusters' : 'instances'}/${id}/kdb/${encodePath(path)}`, {
    method: 'PUT',
    headers: {
      'Content-Type': 'text/plain',
    },
    body: value,
  }).then(response => response.json()),
})

export const setClusterKey = (id, path, value) => setKey(id, path, value, true)
