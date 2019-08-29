export const DBConfig = {
    name: 'eh_db',
    version: 1,
    objectStoresMeta: [
      {
        store: 'files',
        storeConfig: { keyPath: 'id', autoIncrement: true },
        storeSchema: []
      }
    ]
  };