exports.getSession = async skId => {
  const stripe = require("stripe")(skId);

  const session = await stripe.checkout.sessions.create({
    payment_method_types: ["card"],
    line_items: [
      {
        name: "T-shirt",
        description: "Comfortable cotton t-shirt",
        images: ["https://example.com/t-shirt.png"],
        amount: 500,
        currency: "gbp",
        quantity: 1
      }
    ],
    success_url: "https://example.com/success",
    cancel_url: "https://example.com/cancel"
  });

  return session;
};
