const paintKey = "paint";
const changeKey = "change";
const showKey = "show";
const keyActions = [paintKey, changeKey, showKey];

const keyColors = [
  "white",
  "yellow",
  "blue",
  "red",
  "green",
  "black",
  "brown",
  "azure",
  "ivory",
  "teal",
  "silver",
  "purple",
  "navy blue",
  "pea green",
  "gray",
  "orange",
  "maroon",
  "charcoal",
  "aquamarine",
  "coral",
  "fuchsia",
  "wheat",
  "lime",
  "crimson",
  "khaki",
  "hot pink",
  "magenta",
  "olden",
  "plum",
  "olive",
  "cyan"
];

exports.elaborateCommand = commandLine => {
  let ret = {
    targetEntities: [],
    sourceEntities: [],
    nextQuestion: null
  };
  const words = commandLine.toLowerCase().split(" ");
  const inters = keyActions.filter(value => words.includes(value));

  if (inters.length > 0) {
    if (inters[0] === paintKey) {
      const colors = keyColors.filter(value => words.includes(value));
      if (colors.length > 0) {
        ret.targetEntities.push(colors[0]);
      } else {
        ret.nextQuestion = "What color do you have in mind?";
      }
    }
  }

  return ret;
};
