function BOWmodel = BOWtrain(data)

BOWmodel = visualindex_build(data.image, 1:length(data.image), false, 'numWords', 100) ;


