Character = {
    hp = 100,
    speed = 3,
    
    new = function()
      local new_character = {}
      setmetatable(new_character, {__index = Character})
      return new_character
    end, -- need this comma
    
    TakeDamage = function(self, damage)
      self.hp = self.hp - damage
    end 
  }
  
  p1 = Character.new()
  p2 = Character.new()
  p1:TakeDamage(2)
  print(p1.hp)