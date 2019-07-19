export const updateObject = (currentObject, updatedObject) => {
  return {
    ...currentObject,
    ...updatedObject
  };
};
